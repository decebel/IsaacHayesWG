#include "ShaftShader.h"
using namespace std;

#define STRINGIFY(x) #x

ShaftShader::ShaftShader()
{
    vertShader = STRINGIFY(
        attribute vec4 position;
        attribute vec2 texcoord;
        uniform mat4 modelViewProjectionMatrix;
        varying vec2 texCoordVar;
        void main(){
            texCoordVar = texcoord;
            gl_Position = modelViewProjectionMatrix * position;
        }
    );

    fragShader = STRINGIFY(
        precision highp float;
        //
        // Sampler uniforms
        //
        uniform sampler2D lightMap;
        uniform sampler2D scene;
        //
        // Uniforms set from app
        //
        uniform vec2 lightPosition;
        uniform float decay;
        uniform float exposure;
        uniform float density;
        uniform float weight;
        uniform int numSamples;
        const int max_its = 300;
        //
        // Varyings
        //         
        varying vec2 texCoordVar;
        void main()
        {
            vec2 tc = texCoordVar;
            vec4 sceneCol = texture2D(scene,tc);
            vec2 deltaTexCoord = (tc - lightPosition);
            deltaTexCoord *= 1.0 / float(numSamples) * density;
            float illuminationDecay = 1.0;
            vec4 color = texture2D(lightMap, tc) * 0.4;
            for(int i=0; i < max_its ; i++)
            {
                if(i == numSamples) break; // In GLSL ES for loops with non const variables do not work
                tc -= deltaTexCoord;
                vec4 sample = texture2D(lightMap, tc) * 0.4;
                sample *= illuminationDecay * weight;
                color += sample;
                illuminationDecay *= decay;
            }
            gl_FragColor = sceneCol + color;
            //gl_FragColor = texture2D(scene, texCoordVar);
        }
    );
}


ShaftShader::~ShaftShader()
{

}


bool ShaftShader::loadShaders()
{
    ofLog() << "frag: " << ofShader::setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
    ofLog() << "vert: " << ofShader::setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
    ofLog() << "binddef: " << ofShader::bindDefaults();
    ofLog() << "link: " << ofShader::linkProgram();
    return false;

}