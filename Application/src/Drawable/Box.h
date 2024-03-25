#pragma once
#include "DrawableBase.h"
#include <random>

/*
*  Hey Chilli great videos.

Just an addition to the disclaimer. He solves the problem presented, but I don't find this to be even remotely ideal,
namely because of his use of statics which, after initialization, holds onto its resources until the program ends.
Please remember, that should all drawables of a specific type get destroyed that you empty that vector.
Not only that but wrap the vector in a unique_ptr and null it as well.

Or better yet...

Instead of creating a static, create a Bindables class that stores this vector.
You can find a method of serializing it later so that you can use the Bindables class to group Drawables so that you can bind once draw many times to
further improve performance (whatever). And then when all of your drawables are destroyed (level unload) you can release your bindables by simply destroying
the Bindables container. Much cleaner and you don't have all these statics hanging around in memory.

Either way, thanks Chilli for taking the time to educate us. I have benefitted greatly from your selfless act. :) 
 */

class Box : public DrawableBase<Box>
{
public:
    Box( Graphics& gfx,std::mt19937& rng,
        std::uniform_real_distribution<float>& adist,
        std::uniform_real_distribution<float>& ddist,
        std::uniform_real_distribution<float>& odist,
        std::uniform_real_distribution<float>& rdist );
    void Update( float dt ) noexcept override;
    Math::XMMATRIX GetTransformMat() const noexcept override;
private:
    // positional
    float r;
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float theta;
    float phi;
    float chi;
    // speed (delta/s)
    float droll;
    float dpitch;
    float dyaw;
    float dtheta;
    float dphi;
    float dchi;
};
