#include "Includes/Default/Default.frag"
#include "Post-Processing/Post-Processing.frag"

uniform sampler2D COLOR_BUFFER;

void main() {
    FragColor = texture(COLOR_BUFFER, FSIn.UV);
}
