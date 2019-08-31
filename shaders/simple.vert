#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec4 vColor;

uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform float timeElapsedInMillis;


mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}


void main(void)
{
    float seconds = timeElapsedInMillis / 1000.0f;
    vec4 rotatedPosition = vec4(position, 1.0) * rotationMatrix(vec3(1.0, 0.0, 1.0), seconds);
    vec4 totalOffset = vec4(sin(seconds), cos(seconds), 0.0f, 0.0);
    vec4 objectPosition = rotatedPosition + totalOffset;
    gl_Position = worldToView * modelToWorld * objectPosition;
    vColor = vec4(color, 1.0);
}
