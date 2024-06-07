// class SuperClass
// {
//     public:

//         SuperClass(int foo)
//         {
//             // do something with foo
//         }
// };

// class SubClass : public SuperClass
// {
//     public:

//         SubClass(int foo, int bar)
//         : SuperClass(foo)    // Call the superclass constructor in the subclass' initialization list.
//         {
//             // do something with bar
//         }
// };

#ifndef __FACE__
#define __FACE__

class Face : public MatrixPanel_I2S_DMA {
    
    public:
        int x;
        
        Face(HUB75_I2S_CFG mxconfig, int bar) : MatrixPanel_I2S_DMA(mxconfig)  
            {
              x = bar;  
            }

        void lower_triangle(){
            fillTriangle(0, 0, 64, 64, 0, 64, 981);
        }

        void upper_triangle(){
            fillTriangle(0, 0, 64, 64, 64, 0, 63781);
        }
};

#endif