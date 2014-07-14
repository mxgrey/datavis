
#include <iostream>

#include <datavis/Surf3D.hpp>
#include <osgViewer/Viewer>
#include <osg/Light>

using namespace datavis;
using namespace osg;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "What should I draw??" << std::endl;
        return 1;
    }

    ref_ptr<Surf3D> surf = new Surf3D(argv[1]);
    ref_ptr<Group> root = new Group;
    root->addChild(surf);
    
//    ref_ptr<Light> light = new Light;
//    light->setLightNum(4);
//    light->setAmbient(Vec4(0.0,0.0,0.0,1.0));
//    light->setDiffuse(Vec4(1.0,0.0,0.0,1.0));
//    light->setSpecular(Vec4(1.0,1.0,1.0,1.0));
//    light->setPosition(Vec4(0,0,1.5*surf->z_scale,1.0));
//    light->setDirection(Vec3(-1.0,0,-1.0));
    
    
//    ref_ptr<LightSource> source = new LightSource;
//    source->setLight(light);
//    root->addChild(source);
    
//    root->getOrCreateStateSet()->setMode( GL_LIGHT4, osg::StateAttribute::ON);
    
    osgViewer::Viewer viewer;
    viewer.getCamera()->setClearColor(osg::Vec4(1,1,1,1));
    viewer.setSceneData(root);
    viewer.run();

    return 0;
}
