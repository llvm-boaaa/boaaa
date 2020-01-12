#define __STRINGIFY2(X) #X
#define __STRINGIFY(X) __STRINGIFY2(X)

#define __CONCAT(X, Y) X ## Y
#define __XCONCAT(X, Y) __CONCAT(X, Y)
#define __CONCAT3(X, Y, Z) X ## Y ## Z