Write custom plugin in CDTStudio
======

### Create a interface (pure virtual base class) based on QObject
> * Create a c++ class (PlugInterface), which contains a pure virtual function (interface).
> * Add the macro Q_DECLARE_INTERFACE at the end of the class file. The first parameter of the macro is the name of class PlugInterface. The second parameter is the description and version number.

### Implement interface
> * Create a plugin class (PluginInstance)，which implement the interface in PlugInterface.
> * Add macro Q_OBJECT and Q_INTERFACES in header file.
> * At the end of the cpp file, add macro Q_EXPORT_PLUGIN2, whose first parameter is the name of plugin and the second one is the name of the class name of the plugin.

### Invoke in main app.
> * Load plugins by QPluginLoader class.
> * Invoke functions in plugins.