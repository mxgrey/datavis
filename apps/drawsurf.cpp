
#include <iostream>

#include <datavis/Surf3D.hpp>
#include <osgViewer/Viewer>

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
    
    osgViewer::Viewer viewer;
    viewer.getCamera()->setClearColor(osg::Vec4(0,0,0,1));
    viewer.setSceneData(root);
    viewer.run();

    return 0;
}
