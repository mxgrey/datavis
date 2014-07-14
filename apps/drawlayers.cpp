
#include <iostream>

#include "datavis/DrawLayer.hpp"
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>

using namespace datavis;
using namespace osg;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "What should I draw??" << std::endl;
        return 1;
    }

    ref_ptr<DrawLayer> layers = new DrawLayer(argv[1], Vec4(0,0,1,0.9), Vec4(1,0,0,0.9));
    std::vector<size_t> indices; indices.push_back(0); indices.push_back(layers->data.size()-1);
    layers->draw(indices);
    ref_ptr<Group> root = new Group;
    root->addChild(layers);
    
    osgViewer::Viewer viewer;
    viewer.getCamera()->setClearColor(osg::Vec4(1,1,1,1));
    viewer.setSceneData(root);
    ref_ptr<osgGA::TrackballManipulator> cm = new osgGA::TrackballManipulator;
    cm->setHomePosition(layers->mid+Vec3(0,0,0.5), 
                        layers->mid-Vec3(0,0,layers->mid[2]),
                        Vec3(0,-1,0));
    viewer.setCameraManipulator(cm);
    viewer.run();
    
    return 0;
}
