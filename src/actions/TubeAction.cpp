/*
 *  TubeAction.cpp
 *
 *  Copyright (c) 2013, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */
#include "TubeAction.h"

namespace itg
{
    void TubeAction::setRadius(float radius)
    {
        ofVec3f normal = ofVec3f(0, 0, 1);
        for (unsigned i = 0; i < resolution; ++i)
        {
            float theta = TWO_PI * (i % resolution) / (float)resolution;
            vertices.push_back(ofVec3f(radius * sin(theta), 0, radius * cos(theta)));
            normals.push_back(normal);
            normal.rotate(TWO_PI / resolution, ofVec3f(0, 1, 0));
        }
    }
    
    Branch::Ptr TubeAction::step(Branch::Ptr branch, ofMesh& mesh)
    {
        Branch::Ptr newBranch = TransformAction::step(branch, mesh);
        ofMatrix4x4 normalMatrix = inverseTranspose(newBranch->getTransform());
        for (unsigned i = 0; i < resolution; ++i)
        {
            mesh.addVertex(vertices[i] * newBranch->getTransform());
            mesh.addNormal(normals[i] * normalMatrix);
        }
        // if not first slice, add triangle
        if (mesh.getNumVertices() != resolution)
        {
            unsigned prevLayerIdx = mesh.getNumVertices() - 2 * resolution;
            unsigned currLayerIdx = mesh.getNumVertices() - resolution;
            for (unsigned i = 0; i < resolution; ++i)
            {
                mesh.addIndex(prevLayerIdx + i);
                mesh.addIndex(prevLayerIdx + (i + 1) % resolution);
                mesh.addIndex(currLayerIdx + i);
                
                mesh.addIndex(currLayerIdx + i);
                mesh.addIndex(prevLayerIdx + (i + 1) % resolution);
                mesh.addIndex(currLayerIdx + (i + 1) % resolution);
            }
        }
        return newBranch;
    }
    
    void TubeAction::load(ofxXmlSettings& xml, const string& tagName, unsigned tagIdx)
    {
        TransformAction::load(xml, tagName, tagIdx);
        setResolution(xml.getAttribute(tagName, "resolution", 20, tagIdx));
        setRadius(xml.getAttribute(tagName, "radius", 5, tagIdx));
    }
}
