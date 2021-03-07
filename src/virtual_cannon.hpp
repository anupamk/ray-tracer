#ifndef SRC_VIRTUAL_CANNON_HPP__
#define SRC_VIRTUAL_CANNON_HPP__

#include "tuple.hpp"

/*
 * an environment contains both gravity and wind.
 **/
struct env_params {
	raytracer::tuple gravity_;
	raytracer::tuple wind_;

	env_params(raytracer::tuple gravity, raytracer::tuple wind)
	    : gravity_(gravity)
	    , wind_(wind)
	{
	}

	raytracer::tuple gravity() const
	{
		return this->gravity_;
	}

	raytracer::tuple wind() const
	{
		return this->wind_;
	}
};

/*
 * a projectile has both a position and a velocity
 **/
struct projectile {
	raytracer::tuple pos_;
	raytracer::tuple velocity_;

	projectile(raytracer::tuple position, raytracer::tuple velocity)
	    : pos_(position)
	    , velocity_(velocity)
	{
	}

	raytracer::tuple position() const
	{
		return this->pos_;
	}

	raytracer::tuple velocity() const
	{
		return this->velocity_;
	}
};

projectile tick(env_params env, projectile proj);

#endif // SRC_VIRTUAL_CANNON_HPP__
