#include "keela-widgets/AravisController.h"

#include <spdlog/spdlog.h>

namespace Keela {

    AravisController::AravisController(ArvCamera* camera) : aravis_camera(camera) {
        spdlog::info("AravisController initialized with camera pointer: {}", fmt::ptr(camera));
        aravis_camera = camera;
    }

    void AravisController::set_aravis_camera(ArvCamera* camera) {
        spdlog::info("Setting AravisController camera pointer to: {}", fmt::ptr(camera));
        aravis_camera = camera;
    }

    std::pair<double, double> AravisController::get_gain_range() const {
        if (aravis_camera == nullptr) {
            spdlog::warn("Gain control not supported");
            auto nan = std::numeric_limits<double>::quiet_NaN();
            return {nan, nan};
        }

        spdlog::debug("Querying gain range from camera hardware via ArvCamera object");
        
        double min_gain, max_gain;
        GError* error = nullptr;
        // Query the actual hardware gain limits
        arv_camera_get_gain_bounds(aravis_camera, &min_gain, &max_gain, &error);
        if (error == nullptr) {
            spdlog::info("Queried hardware gain range from camera: {:.1f} to {:.1f} dB", min_gain, max_gain);
        } else {
            spdlog::warn("Error querying gain range from camera: {}", error->message);
            g_error_free(error);
        }

        return {min_gain, max_gain};
    }

    std::pair<double, double> AravisController::get_exposure_time_range() const {
        if (aravis_camera == nullptr) {
            spdlog::warn("Exposure time control not supported");
            auto nan = std::numeric_limits<double>::quiet_NaN();
            return {nan, nan};
        }

        spdlog::debug("Querying exposure time range from camera hardware via ArvCamera object");

        double min_exposure, max_exposure;
        GError *error = nullptr;
        // Query the actual hardware exposure time limits
        arv_camera_get_exposure_time_bounds(aravis_camera, &min_exposure, &max_exposure, &error);
        if (error == nullptr) {
            spdlog::info("Queried hardware exposure time range from camera: {:.1f} to {:.1f} us", min_exposure, max_exposure);
        } else {
            spdlog::warn("Error querying exposure time range from camera: {}", error->message);
            g_error_free(error);
        }

        return {min_exposure, max_exposure};
    }

    std::tuple<int, int, int, int> AravisController::get_binning_bounds() const {
        if (aravis_camera == nullptr) {
            spdlog::warn("Binning control not supported");
            auto nan = std::numeric_limits<int>::quiet_NaN();
            return {nan, nan, nan, nan};
        }

        spdlog::debug("Querying binning factor range from camera hardware via ArvCamera object");

        int min_x_binning, max_x_binning;
        int min_y_binning, max_y_binning;
        GError* error_x = nullptr;
        GError* error_y = nullptr;

        // Query the actual hardware binning limits
        arv_camera_get_x_binning_bounds(aravis_camera, &min_x_binning, &max_x_binning, &error_x);
        arv_camera_get_y_binning_bounds(aravis_camera, &min_y_binning, &max_y_binning, &error_y);

        if (error_x == nullptr && error_y == nullptr) {
            spdlog::info("Queried hardware binning factor range from camera: {} to {}", min_x_binning, max_x_binning);
        } else {
            spdlog::warn("Error querying binning factor range from camera: {} {}", error_x ? error_x->message : "no error",
                         error_y ? error_y->message : "no error");
            g_error_free(error_x);
            g_error_free(error_y);
        }

        return {min_x_binning, max_x_binning, min_y_binning, max_y_binning};
    }

    std::pair<int, int> AravisController::get_binning_increments() const {
        if (aravis_camera == nullptr || !arv_camera_is_binning_available(aravis_camera, nullptr)) {
            spdlog::warn("Binning increment query not supported");
            auto nan = std::numeric_limits<int>::quiet_NaN();
            return {nan, nan};
        }

        spdlog::debug("Querying binning factor increment from camera hardware via ArvCamera object");

        GError* error_x = nullptr;
        GError* error_y = nullptr;
        // Query the actual hardware binning increment steps
        int x_increment = arv_camera_get_x_binning_increment(aravis_camera, &error_x);
        int y_increment = arv_camera_get_y_binning_increment(aravis_camera, &error_y);

        if (error_x == nullptr && error_y == nullptr) {
            spdlog::info("Queried hardware binning factor increment from camera: {} (X), {} (Y)", x_increment, y_increment);
        } else {
            spdlog::warn("Error querying binning factor increment from camera: {} {}", error_x ? error_x->message : "no error", error_y ? error_y->message : "no error");
            g_error_free(error_x);
            g_error_free(error_y);
        }

        return {x_increment, y_increment};
    }

    void AravisController::set_gain(double gain) {
        spdlog::info("Setting camera gain to {}", gain);

        GError* error = nullptr;
        arv_camera_set_gain(aravis_camera, gain, &error);

        if (error != nullptr) {
            spdlog::error("Error setting gain on camera: {}", error->message);
            g_error_free(error);
            return;
        }

        // Read back the actual gain value to confirm it was set
        gdouble actual_gain = arv_camera_get_gain(aravis_camera, &error);

        if (error != nullptr) {
            spdlog::error("Error getting gain from camera: {}", error->message);
            g_error_free(error);
            return;
        }

        spdlog::info("Set gain to {:.1f} dB, actual camera gain: {:.1f} dB",
                     gain, actual_gain);
    }

    void AravisController::set_exposure_time(double exposure) {
        spdlog::info("Setting camera exposure time to {}", exposure);

        GError* error = nullptr;
        arv_camera_set_exposure_time(aravis_camera, exposure, &error);

        if (error != nullptr) {
            spdlog::error("Error setting exposure time on camera: {}", error->message);
            g_error_free(error);
            return;
        }
        // Read back the actual exposure time value to confirm it was set
        gdouble actual_exposure = arv_camera_get_exposure_time(aravis_camera, &error);
        if (error != nullptr) {
            spdlog::error("Error getting exposure time from camera: {}", error->message);
            g_error_free(error);
            return;
        }
        spdlog::info("Set exposure time to {:.1f} us, actual camera exposure time: {:.1f} us",
                     exposure, actual_exposure);
    }
    
    void AravisController::set_binning_factors(int binning_factor) {
        set_binning_factors(binning_factor, binning_factor);
    }

    void AravisController::set_binning_factors(int binning_factor_x, int binning_factor_y){
        spdlog::info("Setting camera binning factors to x:{}, y:{}", binning_factor_x, binning_factor_y);

        GError* error = nullptr;
        arv_camera_set_binning(aravis_camera, binning_factor_x, binning_factor_y, &error);

        if (error != nullptr) {
            spdlog::error("Error setting binning factor on camera: {}", error->message);
            g_error_free(error);
            return;
        }

        // Read back the actual binning factor to confirm it was set
        gint actual_binning_x, actual_binning_y;
        arv_camera_get_binning(aravis_camera, &actual_binning_x, &actual_binning_y, &error);

        if (error != nullptr) {
            spdlog::error("Error getting binning factor from camera: {}", error->message);
            g_error_free(error);
            return;
        }

        spdlog::info("Set binning factor to {}x{}, actual camera binning factor: {}x{}",
                     binning_factor_x, binning_factor_y, actual_binning_x, actual_binning_y);
    }

    ArvCamera* AravisController::get_aravis_camera() const {
        return aravis_camera;
    }

}  // namespace Keela