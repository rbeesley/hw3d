# hw3d
Based on ChiliTomatoNoodle's [C++ 3D DirectX Programming tutorial](https://www.youtube.com/playlist?list=PLqCJpWy5Fohd3S7ICFXwUomYW0Wv67pDD). This project follow the same basic milestones, but this implementation adds additional capabilites and architecture not used by the source tutorial.

Checkout the source, including submodules:
`git clone --recurse-submodules -- git@github.com:rbeesley/hw3d.git`

## Build and debug problems

### Error missing file `dxgidebug.dll`
To run a debug build, install the optional feature `Graphics Tools`:
Open Settings using `Win+i`, search for `optional`, select `Manage Optional Features`, add `Graphics Tools`. Release builds should not have this dependency.

### Error missing file `PixelShader.cso`
To be able to run under debug, change:
`Project Property Pages -> Configuration Properties -> Debugging -> Working Directory` : `$(SolutionDir)bin\$(Platform)\$(Configuration)\`. Running the release or debug builds directly, resources should be accessible relativly from the working directory in their expected locations. This change is saved in `"[solution]\hw3d\hw3d.vcxproj.user"`, so it does not persist to version control.