#include "Includes/Default/Default.frag"
#include "Post-Processing/Post-Processing.frag"

uniform sampler2D COLOR_BUFFER;

void main() {
    Kernel(COLOR_BUFFER, FSIn.UV);
    FragColor = texture(COLOR_BUFFER, FSIn.UV);
}
