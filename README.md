# Final Project - A Physically Based Renderer and a Blinn-Phong Renderer

An application for rendering scenes using either Blinn-Phong shading, or physically based shading.

Note that currently **only windows is supported**.

**Access the report that goes alongside this code [here](https://github.com/ej20002015/Final-Report-Write-Up/blob/master/finalReport.pdf)**

## **Originally Hosted On Azure Devops**

This repo was originally hosted on Azure Devops and later migrated to Github, hence the lack of commit history.

## Prerequisites

Visual Studio 2019 must be installed.

## Build Instructions

1. Clone the repository and navigate to the project root directory
2. Run ```call Scripts/GenerateVisualStudioFiles.bat``` to generate the Visual Studio configuration files using Premake
3. Open the ```PBR.sln``` solution file in Visual Studio, and build all the projects

## Run Instructions

- The program can be run:
  - Inside Visual Studio
  - Or by running the executable directly, which is located in the ```Application/bin/Release/Application/``` directory. When doing this, make sure that the working directory is correctly set to ```Application/```
- Not that the ```Release``` build runs significantly faster than the ```Debug``` build

## User Controls

- Operating the camera is done in the following manner:
  - **Moving the mouse + left click** pans the camera
  - **Moving the mouse + right click** rotates the camera
  - **Scroll wheel** zooms the camera in and out
- By default, the Blinn-Phong renderer and scene are running on start up. Switching between the two renderers and scenes is then done in the following manner:
  - Pressing **p** will switch to the physically based renderer and scene
  - Pressing **b** will switch to the Blinn-Phong renderer and scene
- The application window can be resized to whatever size, and the renderers will adjust accordingly

## Building a Scene

An intuitive API is made available to allow the user to define their own scenes. Alternatively, the ```TestSceneFactory``` class can be used to load some pre-made ones. All scene creation code should be invoked from the ```Workspace``` class. Different scenes are created for the two renderers:

### Blinn-Phong Scene

- Blinn-Phong scenes are created by calling the ```BlinnPhongScene::create()``` method
- Models loaded in Blinn-Phong mode are added to the scene using the ```addModel(...)``` method, which takes in a ```Reference``` (a wrapper for ```std::shared_ptr```) to a ```Model``` object, as well as a transform matrix
- Blinn-Phong point lights are added to the scene using the ```addPointLight(...)``` method, which takes in a ```Reference``` to a ```PointLight``` object

### Physically Based Scene

- Physically based scenes are created by calling the ```PBRScene::create()``` method
- Models loaded in PBR mode are added to the scene using the ```addModel(...)``` method, which takes in a ```Reference``` to a ```Model``` object, as well as a transform matrix
- Physically based point lights are added to the scene using the ```addPointLight(...)``` method, which takes in a ```Reference``` to a ```PointLight``` object

## Point Lights

Different point lights are intended for use with the two renderers:

- Blinn-Phong point lights are created using the ```BlinnPhongPointLight::create()``` method
- Physically based point lights are created using the ```PBRPointLight::create()``` method

## Models

Models can be created in two different ways:

### Loading Models From a File

If reading in a model from an external file, the ```Model``` class's constructor can be invoked with two arguments:

- The first argument is an ```std::string``` that contains the filepath
- The second argument is an instance of the ```Model::MaterialModel``` enum that specifies the mode that the model's materials should be loaded with. It should be one of:
  - ```Model::MaterialModel::BLINN_PHONG``` if reading in a 3D model that is intended to be used in a Blinn-Phong scene with the Blinn-Phong renderer. The loader has been validated to work with ```.obj``` files
  - ```Model::MaterialModel::PBR``` if reading in a 3D model that is intended to be used in a physically based scene with the  physically based renderer. The loader has been validated to work with ```.fbx``` files

### Using the Model Factory

Alternatively, a model can be created using the ```ModelFactory``` class. This class generates models with a single simple mesh. Models are created through the model factory by using the ```ModelFactory::create(...)``` method. Two arguments need to be supplied, with a third optional one:

- The first argument is an instance of the ```ModelFactory::MeshType``` enum that specifies the type of mesh to create. It should be one of:
  - ```ModelFactory::MeshType::CUBE``` when wanting to create a cube mesh
  - ```ModelFactory::MeshType::SPHERE``` when wanting to create a UV sphere mesh
- The second argument is a ```Reference``` to a ```Material``` object which may be one of two implementations, depending on what scene and renderer the model will be used with: either a ```BlinnPhongMaterial``` or a ```PBRMaterial```. When setting up these materials prior to passing them to the model factory, it may be necessary to create textures. This can be done using the ```Texture``` class
- The third argument is optional, and only used when a UV sphere is being created. This argument specifies the Level Of Detail (LOD) of the resulting mesh. The default value is 32

## Rendering

Scenes can be submitted to the renderer using the ```Renderer::drawScene(...)``` method, which takes in two arguments:

- The first argument is a ```Reference``` to a ```Scene``` object (the base class of ```BlinnPhongScene``` and ```PBRScene```)
- The second argument is a ```Camera``` object

Switching between the two renderer implementations can be done by calling the ```setRendererType(...)``` method. This method takes in a singular argument that is an instance of the ```Renderer::RendererType``` enum, and specifies the renderer implementation to switch to. It can be either one of: ```Renderer::RendererType::BLINN_PHONG``` or ```Renderer::RendererType::PBR```.
