GLSLCHOME=/usr/local/vulkan/macOS/bin/glslc
GLSLCSCHOOL=/Users/ohakola/goinfre/vulkan/macOS/bin/glslc

if [[ -f "$GLSLCHOME" ]]; then
	$GLSLCHOME shader.vert -o vert.spv
	$GLSLCHOME shader.frag -o frag.spv
else
	if [[ -f "$GLSLCSCHOOL" ]]; then
		$GLSLCSCHOOL shader.vert -o vert.spv
		$GLSLCSCHOOL shader.frag -o frag.spv
	else
		echo "No shader compilation file found!"
	fi
fi

