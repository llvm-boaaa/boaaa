#ifndef BOAAA_ERROR_H
#define BOAAA_ERROR_H

namespace boaaa
{
	class ErrorStream;

	namespace error
	{
		static ErrorStream* stderr;
	}

	class ErrorStream
	{

	};

	class ErrorReporter
	{
		void report(ErrorStream stream);
	};


}

#endif //!BOAAA_ERROR_H