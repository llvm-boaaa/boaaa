#undef __STRINGIFY2(X) #X
#undef __STRINGIFY(X) __STRINGIFY2(X)

#undef __CONCAT(X, Y) X ## Y
#undef __XCONCAT(X, Y) __CONCAT(X, Y)
#undef __CONCAT3(X, Y, Z) X ## Y ## Z

#undef BOAAA_STRINGIFY_H