#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef WIN32
#define CORE_EXPORT _declspec(dllimport)
#define GUI_EXPORT _declspec(dllimport)
#define ANALYSIS_EXPORT _declspec(dllimport)
#else
#define CORE_EXPORT
#define GUI_EXPORT
#define ANALYSIS_EXPORT
#endif

#endif // GLOBAL_H
