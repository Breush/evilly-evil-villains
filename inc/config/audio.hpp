#pragma once

namespace config
{
    struct Audio
    {
        //! Load the corresponding config file.
        Audio();

        //! Save the data to the corresponding config file.
        void save();

        float globalRelVolume = 0.9f;   //!< Volume factor from 0.f to 1.f.
        float musicVolume = 60.f;       //!< Volume from 0 to 100.
        float soundVolume = 80.f;       //!< Volume from 0 to 100.

        float effectiveMusicVolume = 0.9f * 60.f;   //!< Computed effective volume (taking global in account).
        float effectiveSoundVolume = 0.9f * 80.f;   //!< Computed effective volume (taking global in account).
    };
}

