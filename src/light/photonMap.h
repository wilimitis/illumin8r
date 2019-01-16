#pragma once

#include <vector>
#include "light.h"
#include "../object/object.h"

// TODO: Extract CausticPhotonMap and GlobalPhotonMap.
class PhotonMap {
  public:
    class Photon {
      public:
        glm::vec3 position;
        glm::vec3 power;
        int axis;
    };

    class PhotonNode {
      public:
        Photon* photon;
        PhotonNode* left;
        PhotonNode* right;

        PhotonNode(std::vector<Photon*> &photons);
    };

    void emitPhoton(
      Ray &ray,
      const std::vector<Object*> &objects,
      Photon* photon,
      bool requiresSpecularHit);
    void emitPhotons(
      const std::vector<Light*> &lights,
      const std::vector<Object*> &objects,
      bool requiresSpecularHit,
      int count);

    static int photonCount;
    static int photonSearchCountMinimum;
    static float photonSearchDistanceSquared;
    
    static void getNearest(
      std::vector<Photon*> &photons,
      PhotonNode* photonNode,
      glm::vec3 position);

    std::vector<Photon*> photons;
    PhotonNode* photonNode;

    void init(
      const std::vector<Light*> &lights,
      const std::vector<Object*> &objects,
      bool requiresSpecularHit);
};
