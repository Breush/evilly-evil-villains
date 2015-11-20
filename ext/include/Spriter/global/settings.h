#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>

#include <string>

// Fix for to_string not defined in mingw...
#if defined(__GNUC__)
#if defined(__MINGW32__)
namespace std
{
	template<typename T>
	std::string to_string(const T& value)
	{
		return std::string();
	}
}
#endif
#endif

namespace SpriterEngine
{

	typedef void(*ErrorFunctionPointer)(const std::string &errorMessage);

	class Settings
	{
	public:
		static bool renderDebugBoxes;
		static bool renderDebugPoints;
		static bool renderDebugBones;

		static bool enableDebugBones;

		static void simpleError(const std::string &errorMessage);

		static void nullError(const std::string &errorMessage);
		
		static void error(const std::string &errorMessage);

		static void setErrorFunction(ErrorFunctionPointer errorFunction);

		static void suppressErrorOutput(bool suppress = true);

	private:
		static ErrorFunctionPointer errFunction;
		static ErrorFunctionPointer previousErrorFunction;
	};

}
#endif // SETTINGS_H
