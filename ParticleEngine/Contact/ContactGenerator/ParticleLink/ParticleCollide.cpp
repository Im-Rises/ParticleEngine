#include "ParticleCollide.h"

void ParticleCollide::addCollider(ParticleCollider particleCollider) {

}

int ParticleCollide::addContact(ParticleContact *particleContact, unsigned int limit, unsigned int current) {
    for (int i = 0; i < m_colliders.size(); i++) {
        for (int j = i + 1; j < m_colliders.size(); j++) {
            if (current < limit) {
                Particle *particle0 = m_colliders[i].getParticle();
                Particle *particle1 = m_colliders[j].getParticle();
                float distance = particle0->getPosition().distance(particle1->getPosition());
                float sumRadius = m_colliders[i].getRadius() + m_colliders[j].getRadius();
                if (distance < sumRadius) {
                    particleContact[current].setPenetration(sumRadius - distance);
                    particleContact[current].setElasticity(elasticity);
                    Vector3d normalParticle = particle0->getPosition() - particle1->getPosition();
                    particleContact[current].setContactNormal(normalParticle);
                    current += 1;
                }
            } else {
                return current;
            }

        }
    }
    return current;
}


