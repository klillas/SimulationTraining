#version 450

// Input buffer
layout(binding = 0) buffer InputBuffer {
    float inputValue;
};

// Output buffer
layout(binding = 1) buffer OutputBuffer {
    float outputValue;
};

void main() {
    // Square the input value
    outputValue = inputValue * inputValue;
}
