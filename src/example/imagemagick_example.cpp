#include <Magick++.h>
#include <iostream>
using namespace std;
using namespace Magick;
int old_main(int argc,char **argv)
{
    InitializeMagick(*argv);

    // Construct the image object. Seperating image construction from the
    // the read operation ensures that a failure to read the image file
    // doesn't render the image object useless.
    Image l_leaf, d_leaf, trunk, branch;

    Image composite(Geometry(250,200),"white");

    try
    {
        // Read a file into image object
        l_leaf.read( "light_leaf.gif" );
        l_leaf.backgroundColor("transparent");
        l_leaf.transparent("black");

        d_leaf.read( "dark_leaf.gif" );
        d_leaf.backgroundColor("transparent");
        d_leaf.transparent("black");


        trunk.read( "trunk.gif" );
        trunk.transparent("black");

        branch.read( "branch.gif" );
        branch.transparent("black");

        // Resize trunk to be as big as it should
        int height = 150;
        int width = 35;
        trunk.resize(Geometry("35x150!"));
        composite.composite(trunk,107,50,OverCompositeOp);

        // Add branches
        Image branch1(branch);
        branch1.resize(Geometry("100x23!"));
        branch1.rotate(-17.0);
        composite.composite(branch1,125,110,OverCompositeOp);
        Image branch2(branch);
        branch2.resize(Geometry("95x21!"));
        branch2.rotate(-140.0);
        composite.composite(branch2,30,100,OverCompositeOp);

        branch.resize(Geometry("65x29!"));
        branch.rotate(-105.0);
        composite.composite(branch,85,50,OverCompositeOp);

        // Blur branches
        composite.blur(1,2.9);

        // Add leaves
        l_leaf.resize(Geometry("9x17!"));
        d_leaf.resize(Geometry("9x17!"));

        // Cluster at left branch
        for(int i = 0; i < 50; i++)
        {
            cout << 1 << ": Leaf" << i << endl;
            Image leaf(d_leaf);
            leaf.rotate(rand() % 360);
            leaf.transparent("black");
            composite.composite(leaf,(rand() % 45)+10,(rand() % 30)+95,OverCompositeOp);
        }

        // Cluster at right branch
        for(int i = 0; i < 50; i++)
        {
            cout << 2 << ": Leaf" << i << endl;
            Image leaf(d_leaf);
            leaf.rotate(rand() % 360);
            composite.composite(leaf,(rand() % 30)+200,(rand() % 20)+95,OverCompositeOp);
        }


        // Cluster at top
        for(int i = 0; i < 200; i++)
        {
            cout << 3 << ": Leaf" << i << endl;
            Image leaf(d_leaf);
            leaf.rotate(rand() % 360);
            composite.composite(leaf,(rand() % 140)+50,(rand() % 125)+5,OverCompositeOp);
        }
        for(int i = 0; i < 50; i++)
        {
            cout << 3 << ": Leaf" << i << endl;
            Image leaf(l_leaf);
            leaf.rotate(rand() % 360);
            composite.composite(leaf,(rand() % 140)+50,(rand() % 125)+5,OverCompositeOp);
        }
        for(int i = 0; i < 100; i++)
        {
            cout << 3 << ": Leaf" << i << endl;
            Image leaf(d_leaf);
            leaf.rotate(rand() % 360);
            composite.composite(leaf,(rand() % 140)+50,(rand() % 125)+5,OverCompositeOp);
        }
        for(int i = 0; i < 20; i++)
        {
            cout << 3 << ": Leaf" << i << endl;
            Image leaf(l_leaf);
            leaf.rotate(rand() % 360);
            composite.composite(leaf,(rand() % 140)+50,(rand() % 125)+5,OverCompositeOp);
        }
        for(int i = 0; i < 50; i++)
        {
            cout << 3 << ": Leaf" << i << endl;
            Image leaf(d_leaf);
            leaf.rotate(rand() % 360);
            composite.composite(leaf,(rand() % 140)+50,(rand() % 125)+5,OverCompositeOp);
        }
        for(int i = 0; i < 5; i++)
        {
            cout << 3 << ": Leaf" << i << endl;
            Image leaf(l_leaf);
            leaf.rotate(rand() % 360);
            composite.composite(leaf,(rand() % 140)+50,(rand() % 125)+5,OverCompositeOp);
        }

        // Write the image to a file
        composite.write( "tree.jpg" );
    }
    catch( Exception &error_ )
    {
        cout << "Caught exception: " << error_.what() << endl;
        return 1;
    }
    return 0;
}
