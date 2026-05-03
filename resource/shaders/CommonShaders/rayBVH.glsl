#ifndef RAYBVH_GLSL
#define RAYBVH_GLSL

//to check if ray hit bbox (AABB Slab Algorithm)
//ray formula: p(t) = o + t * d
//t = (p(t) - o)/d
bool RayAABBHit(Ray ray, vec3 bmin, vec3 bmax, float tMaxLimit, out float tmin_out){
    float tmin = -1e30; //allow negative t
    float tmax = tMaxLimit;

    for (int i = 0; i < 3; i++){
        float o = ray.origin[i];
        float d = ray.dir[i];

        if (abs(d) < 1e-8){
            if (o < bmin[i] || o > bmax[i])
                return false;
            continue;
        }

        float invD = 1.0/d;
        float t0 = (bmin[i]-o)*invD;
        float t1 = (bmax[i]-o)*invD;
        if (t0 > t1){
            float tmp = t0; t0 = t1; t1 = tmp;
        }

        tmin = max(tmin, t0);
        tmax = min(tmax, t1);

        if (tmin > tmax) return false;
        //if (tmin > tMaxLimit) return false;
    }

    //if (tmax < 0.0) return false; // box 在 ray 后面
    float tnear = max(tmin, 0.0);
    if(tmax < tnear) return false;

    tmin_out = tnear;
    return true;
}

void RayTraceWithBVH(Ray ray, inout HitInfo hitInfo_closest){
    int stack[256];
    int stack_count = 0;

    stack[stack_count] = 0; //put root node into the stack
    stack_count = 1;

    while (stack_count > 0)
    {
        stack_count--;
        int node_idx = stack[stack_count];
        BVHNode node = sboBVHNode.nodes[node_idx];
        int start = node.tri_start;
        int count = node.tri_count;

        if (count > 0){ //leaf
            for(int i = start; i < start+count; i++){
                uint triangleReorderIndex = sboTriangleReorderIndex.indices[i];

                uint idx0 = sboTriangleIndex.indices[ (triangleReorderIndex * 3) + 0 ];
                uint idx1 = sboTriangleIndex.indices[ (triangleReorderIndex * 3) + 1 ];
                uint idx2 = sboTriangleIndex.indices[ (triangleReorderIndex * 3) + 2 ];

                Triangle tri;
                tri.a = sboTriangleVertex.vertices[idx0].position;
                tri.b = sboTriangleVertex.vertices[idx1].position;
                tri.c = sboTriangleVertex.vertices[idx2].position;
                tri.material_id = sboTriangleVertex.vertices[idx0].material_id;

                HitInfo hitInfo_current = RayTriangleHitCheck(ray, tri);
                if(hitInfo_current.didHit && hitInfo_closest.didHit){ //current hit and previous hit
                    if(hitInfo_current.dst < hitInfo_closest.dst) hitInfo_closest = hitInfo_current;
                }else if(hitInfo_current.didHit){ //current hit and no previous hit
                    hitInfo_closest = hitInfo_current;
                }
            }
        }else{ //non-leaf
            float tL, tR;
            bool hitL = false, hitR = false;

            //float tLimit = hitInfo_closest.didHit ? hitInfo_closest.dst : 1e30;
            float tLimit = hitInfo_closest.didHit ? (hitInfo_closest.dst + 1e-4) : 1e30;

            if (node.left >= 0)  hitL = RayAABBHit(ray, sboBVHNode.nodes[node.left].bbox_min,sboBVHNode.nodes[node.left].bbox_max,tLimit,tL);
            if (node.right >= 0) hitR = RayAABBHit(ray, sboBVHNode.nodes[node.right].bbox_min,sboBVHNode.nodes[node.right].bbox_max,tLimit,tR);
            
            if (hitL && hitR){
              if (tL < tR){
                    stack[stack_count++] = node.right;
                    stack[stack_count++] = node.left;
                } else {
                    stack[stack_count++] = node.left;
                    stack[stack_count++] = node.right;
                }
            }
            else if(hitL) stack[stack_count++] = node.left;
            else if(hitR) stack[stack_count++] = node.right;
        }
    }
}

#endif