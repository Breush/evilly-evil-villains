#include "override/filefactory.h"

#include "override/imagefile.h"
#include "override/soundfile.h"

#include "override/spriterfileloader.h"

namespace SpriterEngine
{

	FileFactory::FileFactory()
	{
	}

	ImageFile * FileFactory::newImageFile(const std::string &initialFilePath, point initialDefaultPivot)
	{
		return new ImageFile(initialFilePath, initialDefaultPivot);
	}

	SoundFile * FileFactory::newSoundFile(const std::string &initialFilePath)
	{
		return new SoundFile(initialFilePath);
	}

	SpriterFileLoader * FileFactory::newScmlLoader()
	{
		return new SpriterFileLoader();
	}

	SpriterFileLoader * FileFactory::newSconLoader()
	{
		return new SpriterFileLoader();
	}

}
