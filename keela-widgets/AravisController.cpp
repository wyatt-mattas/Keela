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
        double min_gain = 0.0;
        double max_gain = 0.0;

        if (aravis_camera == nullptr) {
            spdlog::warn("Gain control not supported");
            return {min_gain, max_gain};
        }

        spdlog::debug("Querying gain range from camera hardware via ArvCamera object");

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
        double min_exposure = 0.0;
        double max_exposure = 0.0;

        if (aravis_camera == nullptr) {
            spdlog::warn("Exposure time control not supported");
            return {min_exposure, max_exposure};
        }

        spdlog::debug("Querying exposure time range from camera hardware via ArvCamera object");

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

    ArvCamera* AravisController::get_aravis_camera() const {
        return aravis_camera;
    }

}  // namespace Keela