#ifndef ARAVISCONTROLLER_H
#define ARAVISCONTROLLER_H

#include <aravis-0.8/arv.h>

#include <utility>  // pair

namespace Keela {
    /**
     * Controls Aravis camera operations and settings
     *
     * This class abstracts all Aravis-specific camera controls,
     * keeping camera management separate from pipeline management.
     */
    class AravisController {
    public:
        explicit AravisController(ArvCamera* camera);

        ~AravisController() = default;

        void set_aravis_camera(ArvCamera* camera);

        /** Query hardware capabilities */

        /**
         * Get the range of gain values supported by the camera hardware
         *
         * Returns {nan, nan} if no aravis camera is available
         */
        std::pair<double, double> get_gain_range() const;

        /**
         * Get the range of exposure time values supported by the camera hardware
         *
         * Returns {nan, nan} if no aravis camera is available
         */
        std::pair<double, double> get_exposure_time_range() const;

        /**
         * Get the range of binning factor values supported by the camera hardware
         * 
         * Returns a tuple of {min_x_binning, max_x_binning, min_y_binning, max_y_binning}
         * 
         * Returns {nan, nan, nan, nan} if no aravis camera is available
         */
        std::tuple<int, int, int, int> get_binning_bounds() const;

        /**
         * Get the increment steps for binning factor values supported by the camera hardware
         *
         * Returns {nan, nan} if no aravis camera is available
         */
        std::pair<int, int> get_binning_increments() const;


        /** Control hardware settings */

        void set_gain(double gain);

        void set_exposure_time(double exposure);

        void set_binning_factors(int binning_factor);
        void set_binning_factors(int binning_factor_x, int binning_factor_y);

    private:
        ArvCamera* aravis_camera = nullptr;

        ArvCamera* get_aravis_camera() const;
    };
}  // namespace Keela

#endif  // ARAVISCONTROLLER_H
