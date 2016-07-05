L3++: Lightweight Logging Library for C++
=====

L3++ is a self-contained, single-header, cross-platform logging library for C++.

The main goals for this library are simplicity, modularity and ease of use.
This library is released under the MIT License.

Copyright (C) 2015 Gereon Kremer


Concepts
=====

L3++ is based on the following conceptual components:

* RecordInfo: A record info stores auxiliary information of a log message like the filename, line number and function name where the log message was emitted.
* Logger: A logger categorizes log messages, usually according to logical components or modules in the source code.
* Sink: A sink represents a logging output, for example the terminal or a log file.
* Formatter: A formatter is associated with a sink and converts a log message into an actual string.


Log levels
-----
The following log levels exist (from `l3pp::LogLevel`):
* ALL
* TRACE
* DEBUG
* INFO
* WARN
* ERR
* FATAL
* OFF

Hierarchical Loggers
-----
Loggers are hierarchically structured strings like `"app.module.submodule"`. In this example, `submodule` is considered a sublogger of `module` and `app` the parent of `module`. A sublogger implicitly inherits all sinks and the log level of the parent, unless explicitly configured otherwise.

The hierarchical tree of loggers always contains the root logger at the top. The root logger itself does not have a parent, nor a name. It can be accessed via `l3pp::Logging::getRootLogger()`.

Each logger is assigned a log level, or is configured to inherit the log level of the parent (with the special log level `l3pp::LogLevel::INHERIT`). Note that the root logger cannot inherit a log level. Any log entry with a lower level is filtered out and will not be logged.

A logger can also be assigned one or more sinks. By default, a logger will log to both the sinks of its parent, as well as its own sinks. Should a logger only use it own sinks, it should be set to non-additive (using `Logger::setAdditive(false)`).

Sinks
-----
A sink provides an output for loggers. Loggers may define multiple sinks, and sinks may be shared between loggers. Sinks are associated with a formatter and a log level. The log level specifies the minimum level of a message for it to be output (independent of the log level of a logger), and by default permits all log messages. A formatter formats the log messages before being output. By default, a simple formatter is used which prints the log level, message and a newline, but other formatters can be specified.

Formatters
-----
A formatter shapes a log message before being sent to its final destination. A non-configurable simple formatter exists, as well as a template-based formatter. The latter specifies the format of a message by means of its template arguments, see `l3pp::makeTemplateFormatter`.


Basic Usage
=====

A logger object can be accessed via `l3pp::Logger::getLogger()` or `l3pp::Logger::getRootLogger()`. By default, the root logger does not output anywhere. Therefore, a sink should be added. An initial configuration may look like this:

    l3pp::Logger::initialize();
    l3pp::SinkPtr sink = log4carl::StreamSink::create(std::clog);
    l3pp::Logger::getRootLogger()->addSink(sink);
    l3pp::Logger::getRootLogger()->setLevel(log4carl::LogLevel::INFO);

In this demo, a single sink is created that passes log messages to the standard logging stream `std::clog`. All messages must have at least level `LVL_INFO` before being printed.

The actual logging is performed using a handful of macros.
These macros


Considerations
=====

Performance
-----
While the use of hierarchical loggers and multiple sinks with associated formatters gives a lot of flexibility, it comes at a certain price. As the configuration is done at runtime (and may even change at runtime), the question whether a certain message is printed can only be answered at runtime. Therefore, every message, whether you will ever see it or not, has to pass through the logger and cost runtime.

To mitigate this, we suggest the following:

Create a preprocessor flag (like `ENABLE_LOGGING`) and define your own set of logging macros.
If this flag is defined, make your macros forward to the `L3PP_LOG_*` macros.
If this flag is not defined, make your macros do nothing.


Multiple usages in the same project
-----
Assume you have an application that uses L3++ for logging as well as some other library that also uses L3++.
L3++ will play nicely in this scenario (partly, it was designed for this case).

However, you should take care of a few things:
* Colliding loggers: Prefix your loggers with some unique prefix.
* Colliding macros: If you implement the aforementioned `ENABLE_LOGGING` macro, prefix your macros with your project name. Otherwise, these macros will collide.


Implementation Details
=====

Sinks
-----
A sink is a class that provides some `log` method. Any class that inherits from `l3pp::Sink` can be used.

As of now, two implementations are available: 
* FileSink: Writes to a output file.
* StreamSink: Writes to any given `std::ostream`, for example to `std::cout`.

Formatters
-----
A formatter is a functor that given a log entry, provides a formatted string. The base class `Formatter` provides some very simple formatting, whereas `TemplateFormatter` provides more control over the shape. Internally, a `TemplateFormatter` streams its arguments to a stream before constructing the string. The special types `FieldStr` and `TimeStr` can be used to format particular attributes of a log entry.
