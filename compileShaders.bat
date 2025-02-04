set glslc=C:\VulkanSDK\1.4.304.0\Bin\glslc.exe
%glslc% shader.vert -o vert.spv
%glslc% --target-spv=spv1.5 shader.frag -o frag.spv
%glslc% --target-spv=spv1.5 shader.rgen -o rgen.spv
%glslc% --target-spv=spv1.5 shader.rmiss -o rmiss.spv