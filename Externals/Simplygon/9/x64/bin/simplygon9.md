# Simplygon 9.1 Release Notes

## Latest Simplygon 9.1 release 9.1.22400.0
* 3ds Max/Maya
  - Added TextureDirectory button and browser
  - Fixed issue when passing TextureOutputDirectory between UI and plug-in
* Unity
  - Fix for material names in GeometryDataCaster
  - Support for prefab creation
* FBX
  - Read material channel colors from FBX files
* UE4
  - Fixed hardcoded path to SimplygonSDK inside the Simplygon module. The plugin should now work properly when putting it in project plugins folder.
  - Fixed plugin compile issues when disabling PCH usage
  - Added pipeline error checks. Errors should now be reported to the output log if a simplygon pipeline fails.
  - Removed redundant processing timer. Pipeline execution time is now reported once to the output log.
* Fixed inconsistent UV borders for ReductionProcessor
* Python SDK stability improvements
* Move installer, Uninstall.exe and SimplyUpdate.exe to .NET 5 to avoid random start-up failures
* Fix for field index verification in ShadingGeometryFieldNode

## Older Simplygon 9.1 releases

### 9.1.20400.0
* 3ds Max
  - Preserving unmapped vertex color fields
* Maya
  - Added support for color management
* Unity
  - Texcoords now imported even if not used
  - Support for casting to other file format than PNG
  - Fix for invalid number of submeshes in LOD
  - Import method extended to better support custom scripting
* 3ds Max, Maya and Unity
  - Fix for failing to import cascaded pipelines in UI
* Support for indexed textures with bit depth 1
* Support for Python 3.9

### 9.1.19200.0
* 3ds Max
  - Support for materials with the same name
* 3ds Max and Maya
  - Texture output directory option now available in the UI
  - Select processed meshes option now available in the UI
* glTF
  - Fixed crashes when exporting skinned assets
* New console input parameter for license application -DontSendTelemetry  

### 9.1.17700.0
* Blender
  - Extended BSDF material support
* glTF
  - Added support for KHR_materials_clearcoat  
  - Added support for KHR_materials_transmission
* Fix for incorrect blending when using ETextureBlendType::Over
* Memory usage optimizations
* Added memory tracking tool

### 9.1.16600.0
* Blender
  - Reduction template now works in Blender 2.91 and 2.92
* Unity
  - All Simplygon namespaces are now available in the Simplygon.Unity.EditorPlugin.dll and can be used for scripting
  - The SPL/ini dependency is now removed from the editor plugin src for easy compilation
  - Fix for missing environment variables when running Simplygon in Unity for the first time
* UE4
  - Fixed section info mapping for LODs with no material baking
* `FBXExporter`
  - Fix for preserving scene units
* `GLTFImporter`
  - Default material is now added to avoid crash if materials are missing
* Added STL file format support
* Fixed memory leak in color caster and DDS writer

### 9.1.14300.0
* Welcoming Simplygon Free users to Simplygon 9
* Unity / USD
  - Added support for texture wrapping (repeat, clamp, mirror) to USD
  - Path, WrapMode and SRGB are now texture specific
  - Fixed issue when using Unity's Unlit shader (caused by invalid color values in USD)
  - Fixes for root node rotation and scaling issues
  - Always add base color when using automatic casters
  - Fix for metallic/smoothness automatic casters and material channel name
  - Added check for USD 2.0 installation status
  - Documentation updates
  - Knows issues:
    * Unity.USD.Export ignores SRGB - USD implementation limited to detecting normal maps
    * Unity.USD.Import ignores WrapMode - USD fully support WrapMode
    * Unity.USD does not export offset and tiling - USD does not seem to support these
* Blender
  - Fix crash when processing an asset without basecolor texture in Blender
  - Added warnings for unsupported material channels
  - Added checkboxes to the different reduction targets for StopCondition to function properly
  - Added processing progress tracking
  - Fixed automatic material casters
  - Fixed SingleViewImpostor view direction
  - Material caster deserialization fix
  - Fixes for processing issues with cascaded and non-cascaded processings
  - Documentation updates
* Maya
  - Removed length limit of field names for UVs and vertex colors
* `GLTFExporter`
  - Fix for opacity in basecolor alpha channel
  - Fix crash when exporting asset without basecolor texture
* Add ability to run [Simplygon Grid Agent as a service](/api/concepts/distribution.md#running-as-a-windows-service)
* Add option for [setting FBX file format version](/api/concepts/sceneimporterexporter.md#fbx) when using `FBXExporter`
* Added possibility to zoom images in documentation
* Fix crash when trying to run a remeshing with `ERemeshingMode::Inside` on a mesh without an inside
* Fix crash when processing in Houdini (add support for GlobalAttributes in bgeo files)
* Fix material section info when processing static meshes via LOD recipes in UE4

### 9.1.9100.0
* Support passthrough of animation data in glTF
* Read access violation fix
* Houdini plugin fixes;
  - Fix memory leak in HoudiniExporter
  - Fix for missing temp dir in HDA scripts
  - Added early check for triangulated input in HoudiniImporter
  - Fix for temp dir cleanup
* Changes to `ERemeshingMode::Manual` due to known issues with the implementation in 9.1 (will be fixed in 9.2). See [ERemeshingMode](/api/reference/enums/eremeshingmode.md) for more information.
* UE plugin fixes;
  - Fix, incorrect section info mapping when using reduction pipeline from LOD Recipe
  - Fix, crash when using the plugin without a valid license
  - Fix, material baking settings was not reset correctly when removing all material property casters
  - Fix, reset mapping image settings when material baking is disabled
  - Chart aggregator is now using correct default values (same as aggregation)
  - Added LOD streaming settings to LOD Recipe
* Fix serialization of tangent space global settings
* General Unity plugin improvements related to material mapping, transparency, skinning and impostor processor support. Upgrade to Unity.USD 2.0.
* General Blender plugin improvements including UI cleanup, progress tracking and normal maping fixes etc.
* Improved memory usage for long running processes using Simplygon
* Fixed issue with MaxEdgeLength causing bad outputs. Default is now REAL_MAX and setting removed from integration UIs.

### 9.1.6600.0
* Support for Unity USD package 2.0
* Fixed crash when appending packed geometry with very few vertices
* Fixed issue with OpacityInAlpha in color caster

### 9.1.6200.0
* Support for [running SimplygonGrid over multiple subnets](../api/concepts/distribution.md#bridging-subnets)
* Support for [distributing processings without locally installed license](../installation/envvariables.md#feature-flags)
* Maya - fixed mapping issue related to bones with same name
* Maya - stability improvements
* 3ds Max - stability improvements

### 9.1.5200.0
* glTF texture packing improvements
* Fixed crash in legacy remesher

### 9.1.4100.0
* Blender 2.91 support
* Fix for 3ds Max multi-material mapping issue
* Fix for morph target issue
* Unity impostor fix

### 9.1.3200.0
* Blender fixes
* glTF texture name handling

### Initial release (9.1.2600.0)

#### Major updates
* Standalone UE4 plugin
* New impostor processor
* Autodesk 3ds Max plugin (Max 2021 in preview)
* FASTBuild and Incredibuild support
* New integrations as previews: Blender, Houdini & Unity
* New file formats: USD and Alembic

#### Simplygon API updates
* Ability to use opacity as transparency in material casters
* Foliage processor is now part of impostor processor type: billboard cloud  
* Added posibilty to add meshes to CameraSelectionSets
* Visibility support in new remeshing processor
* Added method to dump a settings object as JSON 
* Added geometry field shading node
* Added missing rotation methods to texture shading node interface

#### Pipeline updates
* Added batch API 
* Added support for metadata in pipeline settings
* Added cascaded pipeline support in pipeline settings path
* Arbitrary pipeline in cascade mode as intermediate
* New setting for unique material names in pipelines

#### Max plugin updates
* Camera support for visibility
* Automatic export of active selection sets
* New impostor pipeline
* Added support for legacy INI settings
* Added support for physical material system in Max 2021 preview

#### Maya plugin updates
* Camera support for visibility 
* Automatic export of active selection sets
* New impostor pipeline
* ShaderFX/Stingray PBS documentation
* Added support for legacy INI settings

#### File format updates
* Support for glTF KHR_texture_transform
* Skinning support for glTF
* glTF: Support for separate wrap modes
* Support for Alembic and USD


