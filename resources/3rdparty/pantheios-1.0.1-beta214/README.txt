pantheios - README
==================

Updated:    7th August 2012



See the introduction in the accompanying doc/html/1.0.1-beta214/index.html
file.

Please feel free to request, nay demand, improvements in any areas
that you feel are deficient. We believe that Pantheios is the best
diagnostic logging API ever created, but we are not so deluded as to think
that our presentational skills are as good. Criticism will be gratefully
received.

If you are interested in commercial services and/or training for
Pantheios, contact Synesis Software, via http://synesis.com.au/contact.html

Information about Pantheios training is available at
http://synesis.com.au/training.html


10 important things to know about Pantheios:
--------------------------------------------

1. It's a diagnostic logging **API** library, not a diagnostic logging
   library

2. It's open-source, and 100% free

3. It depends on several other libraries, but they're also open source and
   100% free

4. It's incredibly efficient, and is faster than all other serious C++
   diagnostic logging libraries by a huge margin (up to two orders of
   magnitude)

5. It is 100% type-safe

6. Selection of logging transport (i.e. "back-end(s)") is done at link-time,
   for very good reason

7. It's highly extensible

8. It's being used by serious commercial enterprises, including very high
   throughput financial systems

9. It's highly portable - including most C++ compilers and most/all flavours
   of UNIX, Linux, Mac OS-X and Windows

10. Pantheios is under active development from a highly motivated team, who
   have the ambition that it will become the diagnostic logging API of
   choice for all C++ programmers who wish to have performance without
   sacrificing robustness or flexibility; the team want to hear from you how
   they can better achieve these aims


Details:
- - - -

1. It's a diagnostic logging **API** library, not a diagnostic logging
   library

   The Pantheios architecture is split into four parts:
    - Application Layer - you use this to create diagnostic logging
	   statements in your application code
    - Core - this is what ties it all together. Apart from initialising the
       core (which happens automatically in C++ compilation units), you
       don't need to do anything with the core
    - Front-end - this is what decides which statements are written out, as
       well as providing the application identity (name) to the Core during
       initialisation. There are several stock Front-ends included in the
       Pantheios distribution, but you may well implement your own
    - Back-end - this is what emits the logging statement (if the front-end
       allows it to be written) to the outside world. There are a number of
       stock Back-ends included in the Pantheios distribution, but you may
       well implement your own

   One criticism that we sometimes get from new users goes along the lines
   of

     " the performance is brilliant, but you don't have all the features of
      log4cxx "

   We know. It's designed to be that way.

   Pantheios is a diagnostic logging API library. The interaction between
   Application Layer, Core and Front-end means that nothing else can come
   anywhere near Pantheios in terms of performance, type-safety or
   extensibility. But it's specifically designed for you to use it _above_
   existing logging libraries that have more features. To do so, you can
   write a simple back-end to wrap, say, log4cxx, and plug it in at
   link-time. That way you get all the performance and safety of Pantheios,
   with the rich feature set of log4cxx. It's win-win!


2. It's open-source, and 100% free

   It uses the widely appreciated (modified-)BSD license.


3. It depends on several other libraries, but they're also open source and
   100% free

   Apart from one library - STLSoft - all the libraries on which Pantheios
   depends are bundled with the Pantheios distribution. These libraries are:

    - xTests  (see http://xtests.org/)
      
      A simple, lightweight unit-/component-testing libary for C
      and C++
      
      This library is used only in the unit-/component-testing

    - b64  (see http://synesis.com.au/software/b64.html)
    
      A simple, exceedingly lightweight implementation of the Base-64
      encoding
      
      This library is used by the pantheios::b64 inserter class

    - shwild  (see http://shwild.org/)

      An implementation (UNIX) shell compatible wildcards.
      
      This library is used only in the unit-/component-testing

   The other library on which Pantheios depends is STLSoft (see
   http://stlsoft.org). STLSoft is *not* an STL replacement, it is an STL
   extension library. Additionally, it provides a number of system and
   compiler discrimination features that Pantheios uses to simplify its
   implementation


4. It's incredibly efficient, and is faster than all other serious C++
   diagnostic logging libraries by a huge margin (up to two orders of
   magnitude)

   See http://pantheios.sourceforge.net/performance.html#sweet-spot if you
   don't believe us


5. It is 100% type-safe

   ... unlike any diagnostic logging library built on C's Streams or C++'s
   IOStreams libraries.


6. Selection of logging transport (i.e. "back-end(s)") is done at link-time,
   for very good reason

   The reason is simple: a diagnostic logging library must be available
   whenever _any_ part of the application code needs it. In C++, it's
   possible to execute a significant amount of functionality during dynamic
   initialisation, so we cannot afford to wait for main() to set up the
   logging.

   The consequence of this is Pantheios' only hard-to-use aspect, that of
   setting up the linking. There are several tutorials available for this
   purpose in the documentation


7. It's highly extensible

   Pantheios ships with a number of stock transports (known as
   "back-end(s)"), including console, Syslog, COM Error Object, speech,
   Windows Debugger, Windows Event Log. You can extend the range with a
   custom back-end by implementing a very simple C-API. The details are in
   the documentation.

   By default, the Pantheios Application Layer understands a large range of
   string types (and types that can be meaningfully converted into strings,
   such as struct tm, FILETIME, struct in_addr, and so on). Pantheios allows
   you to infinitely extend the range of types that may be incorporated into
   logging statements in your application code. There is an extensive
   tutorial on this in the documentation.


8. It's being used by serious commercial enterprises, including very high
   throughput financial systems

   Pantheios is in use by companies in Australia, US, and throughout the
   world.
   
   One notable user in New York contacted Synesis Software
   (see http://synesis.com.au/) in Dec 2007 to design and implement custom
   front-/back-ends to enhance the performance to an insane degree - not one
   processor cycle is wasted - because they believe that their systems will
   need to produce more diagnostic logging output than any other softare
   system in history.

   An NDA prevents us from naming the client, their business, or the nature
   of the customisations, but we can say that they're delighted with the
   system's performance and reliability. They said that Pantheios operates
   with "clock-cycle speed"!


9. It's highly portable - including most C++ compilers and most/all flavours
   of UNIX, Linux, Mac OS-X and Windows

   Pantheios has been tested on a wide range of C++ compilers, and a
   reasonable range of operating systems. In UNIX flavours, there are no
   platform-specific constructs used. Any new compiler/platform will
   require a minimum of enhancement to the STLSoft configuration. The
   development team are always happy to help/advise any such user.


10. Pantheios is under active development from a highly motivated team, who
   have the ambition that it will become the diagnostic logging API of
   choice for all C++ programmers who wish to have performance without
   sacrificing robustness or flexibility; the team want to hear from you how
   they can better achieve these aims

   That pretty much says it all. We know it's the best concept based on the
   best theory, and we're keen to help make it work well for all users
   everywhere.

=============================== End of file ================================
