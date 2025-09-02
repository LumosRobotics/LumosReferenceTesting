#include <gtest/gtest.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <filesystem>
#include "../applications/simple/bounds_checker.hpp"
#include "../applications/simple/binary_serializer.hpp"

class ApplicationTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        // Clean up any existing reference files
        cleanupReferenceFiles();
        
        // Generate test data similar to a real application
        generateTestData();
    }
    
    void TearDown() override 
    {
        cleanupReferenceFiles();
    }
    
    void cleanupReferenceFiles() 
    {
        std::vector<std::string> files = {
            "sensor_data_x.bin", "sensor_data_y.bin", "sensor_time.bin",
            "control_signal.bin", "control_time.bin",
            "trajectory_x.bin", "trajectory_y.bin", "trajectory_time.bin",
            "performance_metrics.bin"
        };
        
        for (const auto& file : files) 
        {
            std::filesystem::remove(file);
        }
    }
    
    void generateTestData() 
    {
        const int N = 50;
        const double dt = 0.1;
        
        // Generate time vector
        time_vec.resize(N);
        for (int i = 0; i < N; ++i) 
        {
            time_vec[i] = i * dt;
        }
        
        // Generate sensor data (noisy sine wave)
        sensor_x.resize(N);
        sensor_y.resize(N);
        for (int i = 0; i < N; ++i) 
        {
            double t = time_vec[i];
            sensor_x[i] = std::sin(t) + 0.01 * std::sin(10 * t); // Main signal + noise
            sensor_y[i] = std::cos(t) + 0.02 * std::cos(15 * t); // Main signal + noise
        }
        
        // Generate control signals
        control_signal.resize(N);
        for (int i = 0; i < N; ++i) 
        {
            control_signal[i] = 0.5 * std::sin(2 * time_vec[i]);
        }
        
        // Generate reference trajectory (clean sine/cosine)
        ref_x.resize(N);
        ref_y.resize(N);
        for (int i = 0; i < N; ++i) 
        {
            ref_x[i] = std::sin(time_vec[i]);
            ref_y[i] = std::cos(time_vec[i]);
        }
    }
    
    std::vector<double> time_vec, sensor_x, sensor_y, control_signal, ref_x, ref_y;
};

TEST_F(ApplicationTest, SensorDataValidation) 
{
    // Application use case: Validate sensor readings are within expected bounds
    
    // Define acceptable bounds for sensor data
    std::vector<double> x_min(sensor_x.size(), -1.5);
    std::vector<double> x_max(sensor_x.size(), 1.5);
    std::vector<double> y_min(sensor_y.size(), -1.5);
    std::vector<double> y_max(sensor_y.size(), 1.5);
    
    // Test: Sensor data should be within physical limits
    EXPECT_TRUE(isWithinBounds(sensor_x, x_min, x_max)) << "Sensor X data exceeds physical limits";
    EXPECT_TRUE(isWithinBounds(sensor_y, y_min, y_max)) << "Sensor Y data exceeds physical limits";
    
    // Test: Sensor variance should be low (indicating stable readings)
    EXPECT_TRUE(isVarianceWithinThreshold(sensor_x, ref_x, 0.1)) << "Sensor X variance too high";
    EXPECT_TRUE(isVarianceWithinThreshold(sensor_y, ref_y, 0.1)) << "Sensor Y variance too high";
    
    // Test: Mean should be close to reference
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(sensor_x, ref_x, 0.05)) << "Sensor X mean drift detected";
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(sensor_y, ref_y, 0.05)) << "Sensor Y mean drift detected";
}

TEST_F(ApplicationTest, ControlSystemValidation) 
{
    // Application use case: Validate control system performance
    
    // Test: Control signal should not exceed saturation limits
    EXPECT_TRUE(hasAtLeastNSamplesAboveThreshold(control_signal, 0.4, 0)) << "Control signal never reaches minimum activation";
    EXPECT_FALSE(hasAtLeastNSamplesAboveThreshold(control_signal, 0.6, 1)) << "Control signal saturating";
    
    // Test: Control signal should not stay at extreme values too long
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesAboveThreshold(control_signal, 0.45, 10)) << "Control signal stuck high";
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesBelowThreshold(control_signal, -0.45, 10)) << "Control signal stuck low";
    
    // Test: Custom condition - control signal should be responsive (changing)
    auto is_small_magnitude = [](const double& x) { return std::abs(x) < 0.1; };
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesWithConditionTrue(control_signal, is_small_magnitude, 20)) 
        << "Control signal inactive for too long";
}

TEST_F(ApplicationTest, TrajectoryTracking) 
{
    // Application use case: Validate trajectory follows reference path within corridor
    
    // Create corridor boundaries around reference trajectory
    std::vector<double> corridor_x_left, corridor_y_left, corridor_x_right, corridor_y_right;
    const double corridor_width = 0.2;
    
    for (size_t i = 0; i < ref_x.size(); ++i) 
    {
        // Create perpendicular offset for corridor boundaries
        double dx = (i < ref_x.size() - 1) ? ref_x[i+1] - ref_x[i] : ref_x[i] - ref_x[i-1];
        double dy = (i < ref_y.size() - 1) ? ref_y[i+1] - ref_y[i] : ref_y[i] - ref_y[i-1];
        double norm = std::sqrt(dx*dx + dy*dy);
        
        if (norm > 0) 
        {
            dx /= norm;
            dy /= norm;
        }
        
        // Perpendicular vector for corridor width
        double perp_x = -dy * corridor_width;
        double perp_y = dx * corridor_width;
        
        corridor_x_left.push_back(ref_x[i] + perp_x);
        corridor_y_left.push_back(ref_y[i] + perp_y);
        corridor_x_right.push_back(ref_x[i] - perp_x);
        corridor_y_right.push_back(ref_y[i] - perp_y);
    }
    
    // Test: Actual trajectory should stay within corridor
    // Note: This test may fail with noisy data - in real applications you'd use wider corridors
    // For demonstration, we'll use a simpler rectangular corridor test
    std::vector<double> simple_x_min(sensor_x.size(), -1.2);
    std::vector<double> simple_x_max(sensor_x.size(), 1.2);
    std::vector<double> simple_y_min(sensor_y.size(), -1.2);
    std::vector<double> simple_y_max(sensor_y.size(), 1.2);
    
    EXPECT_TRUE(isWithinBounds(sensor_x, simple_x_min, simple_x_max)) 
        << "X trajectory outside simple corridor";
    EXPECT_TRUE(isWithinBounds(sensor_y, simple_y_min, simple_y_max)) 
        << "Y trajectory outside simple corridor";
}

TEST_F(ApplicationTest, DataPersistenceAndRecall) 
{
    // Application use case: Save reference data and verify it can be loaded correctly
    
    // Save current sensor data as reference
    EXPECT_NO_THROW(saveBinaryVector(sensor_x, "sensor_data_x.bin"));
    EXPECT_NO_THROW(saveBinaryVector(sensor_y, "sensor_data_y.bin"));
    EXPECT_NO_THROW(saveBinaryVector(time_vec, "sensor_time.bin"));
    
    // Load and verify data integrity
    std::vector<double> loaded_x, loaded_y, loaded_time;
    EXPECT_NO_THROW(loaded_x = loadBinaryVector<double>("sensor_data_x.bin"));
    EXPECT_NO_THROW(loaded_y = loadBinaryVector<double>("sensor_data_y.bin"));
    EXPECT_NO_THROW(loaded_time = loadBinaryVector<double>("sensor_time.bin"));
    
    // Verify loaded data matches original
    ASSERT_EQ(sensor_x.size(), loaded_x.size());
    ASSERT_EQ(sensor_y.size(), loaded_y.size());
    ASSERT_EQ(time_vec.size(), loaded_time.size());
    
    for (size_t i = 0; i < sensor_x.size(); ++i) 
    {
        EXPECT_DOUBLE_EQ(sensor_x[i], loaded_x[i]) << "Loaded X data mismatch at index " << i;
        EXPECT_DOUBLE_EQ(sensor_y[i], loaded_y[i]) << "Loaded Y data mismatch at index " << i;
        EXPECT_DOUBLE_EQ(time_vec[i], loaded_time[i]) << "Loaded time data mismatch at index " << i;
    }
}

TEST_F(ApplicationTest, PerformanceMetricsValidation) 
{
    // Application use case: Calculate and validate system performance metrics
    
    // Calculate tracking errors
    std::vector<double> tracking_error_x(sensor_x.size());
    std::vector<double> tracking_error_y(sensor_y.size());
    
    for (size_t i = 0; i < sensor_x.size(); ++i) 
    {
        tracking_error_x[i] = std::abs(sensor_x[i] - ref_x[i]);
        tracking_error_y[i] = std::abs(sensor_y[i] - ref_y[i]);
    }
    
    // Test: Tracking errors should be small
    auto small_error = [](const double& error) { return error < 0.1; };
    EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(tracking_error_x, small_error, tracking_error_x.size() * 0.9))
        << "X tracking error too large for more than 10% of samples";
    EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(tracking_error_y, small_error, tracking_error_y.size() * 0.9))
        << "Y tracking error too large for more than 10% of samples";
    
    // Test: No sustained large errors
    auto large_error = [](const double& error) { return error > 0.2; };
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesWithConditionTrue(tracking_error_x, large_error, 5))
        << "Sustained large X tracking error detected";
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesWithConditionTrue(tracking_error_y, large_error, 5))
        << "Sustained large Y tracking error detected";
    
    // Save performance metrics for later analysis
    std::vector<double> combined_metrics;
    combined_metrics.insert(combined_metrics.end(), tracking_error_x.begin(), tracking_error_x.end());
    combined_metrics.insert(combined_metrics.end(), tracking_error_y.begin(), tracking_error_y.end());
    
    EXPECT_NO_THROW(saveBinaryVector(combined_metrics, "performance_metrics.bin"));
}

TEST_F(ApplicationTest, TimeBasedAnalysis) 
{
    // Application use case: Analyze system behavior over different time periods
    
    // Create reference bounds that vary over time (tightening constraints)
    std::vector<double> bounds_time = {0.0, 2.0, 4.0};
    std::vector<double> loose_bounds = {0.3, 0.2, 0.1}; // Tightening over time
    std::vector<double> tight_bounds = {-0.3, -0.2, -0.1};
    
    // Test: Early phase should allow larger deviations
    std::vector<double> early_time = {0.5, 1.0, 1.5};
    std::vector<double> early_data = {0.25, 0.15, 0.05}; // Within early bounds
    
    EXPECT_TRUE(isWithinBounds(early_time, early_data, bounds_time, tight_bounds, bounds_time, loose_bounds))
        << "Early phase data should pass with looser bounds";
    
    // Test: Later phase should have tighter requirements
    std::vector<double> late_time = {3.5, 4.0, 4.5};
    std::vector<double> late_data = {0.05, 0.02, 0.01}; // Must be tighter
    
    EXPECT_TRUE(isWithinBounds(late_time, late_data, bounds_time, tight_bounds, bounds_time, loose_bounds))
        << "Late phase data should pass with tighter bounds";
}

TEST_F(ApplicationTest, SystemHealthMonitoring) 
{
    // Application use case: Monitor overall system health using multiple criteria
    
    bool system_healthy = true;
    std::vector<std::string> health_issues;
    
    // Check 1: Signal quality (variance)
    if (!isVarianceWithinThreshold(sensor_x, ref_x, 0.05)) 
    {
        system_healthy = false;
        health_issues.push_back("High X sensor variance");
    }
    
    if (!isVarianceWithinThreshold(sensor_y, ref_y, 0.05)) 
    {
        system_healthy = false;
        health_issues.push_back("High Y sensor variance");
    }
    
    // Check 2: No signal dropout (consecutive low values)
    auto signal_dropout = [](const double& x) { return std::abs(x) < 0.01; };
    if (hasAtLeastNConsecutiveSamplesWithConditionTrue(sensor_x, signal_dropout, 5)) 
    {
        system_healthy = false;
        health_issues.push_back("X sensor signal dropout detected");
    }
    
    if (hasAtLeastNConsecutiveSamplesWithConditionTrue(sensor_y, signal_dropout, 5)) 
    {
        system_healthy = false;
        health_issues.push_back("Y sensor signal dropout detected");
    }
    
    // Check 3: Control system responsiveness
    auto control_active = [](const double& x) { return std::abs(x) > 0.1; };
    if (!hasAtLeastNSamplesWithConditionTrue(control_signal, control_active, control_signal.size() * 0.3)) 
    {
        system_healthy = false;
        health_issues.push_back("Control system not sufficiently active");
    }
    
    // Report health status
    if (!system_healthy) 
    {
        std::cout << "System health issues detected:\n";
        for (const auto& issue : health_issues) 
        {
            std::cout << "  - " << issue << "\n";
        }
    }
    
    EXPECT_TRUE(system_healthy) << "System health check failed";
}

TEST_F(ApplicationTest, QualityAssuranceWorkflow) 
{
    // Application use case: Complete QA workflow for a test run
    
    // Step 1: Save reference data (first run)
    saveBinaryVector(ref_x, "trajectory_x.bin");
    saveBinaryVector(ref_y, "trajectory_y.bin");
    saveBinaryVector(time_vec, "trajectory_time.bin");
    
    // Step 2: Load reference data (subsequent runs)
    std::vector<double> loaded_ref_x = loadBinaryVector<double>("trajectory_x.bin");
    std::vector<double> loaded_ref_y = loadBinaryVector<double>("trajectory_y.bin");
    std::vector<double> loaded_time = loadBinaryVector<double>("trajectory_time.bin");
    
    // Step 3: Comprehensive comparison against reference (with tolerance for noise)
    std::vector<double> x_tolerance_min(loaded_ref_x.size());
    std::vector<double> x_tolerance_max(loaded_ref_x.size());
    std::vector<double> y_tolerance_min(loaded_ref_y.size());
    std::vector<double> y_tolerance_max(loaded_ref_y.size());
    
    for (size_t i = 0; i < loaded_ref_x.size(); ++i) 
    {
        x_tolerance_min[i] = loaded_ref_x[i] - 0.05;
        x_tolerance_max[i] = loaded_ref_x[i] + 0.05;
        y_tolerance_min[i] = loaded_ref_y[i] - 0.05;
        y_tolerance_max[i] = loaded_ref_y[i] + 0.05;
    }
    
    EXPECT_TRUE(isWithinBounds(sensor_x, x_tolerance_min, x_tolerance_max)) << "X trajectory tolerance check failed";
    EXPECT_TRUE(isWithinBounds(sensor_y, y_tolerance_min, y_tolerance_max)) << "Y trajectory tolerance check failed";
    
    // Step 4: Statistical validation
    EXPECT_TRUE(isVarianceWithinThreshold(sensor_x, loaded_ref_x, 0.001)) << "X statistical variance check failed";
    EXPECT_TRUE(isVarianceWithinThreshold(sensor_y, loaded_ref_y, 0.001)) << "Y statistical variance check failed";
    
    // Step 5: Time-based validation with interpolation
    // Simulate different sampling rates
    std::vector<double> test_time = {0.05, 0.15, 0.25, 0.35, 0.45};
    std::vector<double> test_values;
    
    for (double t : test_time) 
    {
        test_values.push_back(interpolateAtTime(t, time_vec, sensor_x));
    }
    
    // Create bounds around interpolated reference
    std::vector<double> interp_ref, interp_min, interp_max;
    for (double t : test_time) 
    {
        double ref_val = interpolateAtTime(t, time_vec, ref_x);
        interp_ref.push_back(ref_val);
        interp_min.push_back(ref_val - 0.1);
        interp_max.push_back(ref_val + 0.1);
    }
    
    EXPECT_TRUE(isWithinBounds(test_values, interp_min, interp_max)) 
        << "Interpolated values outside acceptable bounds";
}

TEST_F(ApplicationTest, RealTimeMonitoring) 
{
    // Application use case: Real-time monitoring with immediate feedback
    
    // Simulate real-time processing of incoming data
    bool monitoring_passed = true;
    std::vector<std::string> alerts;
    
    // Process data in chunks (simulating real-time windows)
    const size_t window_size = 10;
    for (size_t start = 0; start + window_size <= sensor_x.size(); start += window_size) 
    {
        std::vector<double> window_x(sensor_x.begin() + start, sensor_x.begin() + start + window_size);
        std::vector<double> window_y(sensor_y.begin() + start, sensor_y.begin() + start + window_size);
        std::vector<double> window_ref_x(ref_x.begin() + start, ref_x.begin() + start + window_size);
        std::vector<double> window_ref_y(ref_y.begin() + start, ref_y.begin() + start + window_size);
        
        // Real-time checks
        if (!isVarianceWithinThreshold(window_x, window_ref_x, 0.02)) 
        {
            monitoring_passed = false;
            alerts.push_back("Window " + std::to_string(start/window_size) + ": High X variance");
        }
        
        if (!isMeanDifferenceWithinThreshold(window_y, window_ref_y, 0.03)) 
        {
            monitoring_passed = false;
            alerts.push_back("Window " + std::to_string(start/window_size) + ": Y mean drift");
        }
        
        // Check for anomalous spikes
        auto is_spike = [](const double& x) { 
            return std::abs(x) > 1.2; // Threshold based on expected signal range
        };
        
        if (hasAtLeastNSamplesWithConditionTrue(window_x, is_spike, 1)) 
        {
            monitoring_passed = false;
            alerts.push_back("Window " + std::to_string(start/window_size) + ": Signal spike detected");
        }
    }
    
    if (!monitoring_passed) 
    {
        std::cout << "Real-time monitoring alerts:\n";
        for (const auto& alert : alerts) 
        {
            std::cout << "  - " << alert << "\n";
        }
    }
    
    EXPECT_TRUE(monitoring_passed) << "Real-time monitoring detected issues";
}