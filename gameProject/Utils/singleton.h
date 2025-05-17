#ifndef SINGLETON_H
#define SINGLETON_H


template <typename T>
class CSingleton
{
protected:
    CSingleton( ) { }
    ~CSingleton( ) { }

    CSingleton( const CSingleton & ) = delete;
    CSingleton & operator=( const CSingleton & ) = delete;

    CSingleton( CSingleton && ) = delete;
    CSingleton & operator=( CSingleton && ) = delete;
public:
    static T & Get( )
    {
        static T pInstance { };
        return pInstance;
    }
};

#endif // SINGLETON_H