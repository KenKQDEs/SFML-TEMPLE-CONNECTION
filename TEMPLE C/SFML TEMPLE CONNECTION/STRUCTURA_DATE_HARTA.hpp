
#include <iostream>
template <typename T, typename P>
struct pereche
{

    T first;
    P second;
    pereche(T ceva, P ceva1) :first(ceva), second(ceva1)
    {}
    pereche() {}

};
template <typename T, typename P>
struct harta_stl
{
    pereche<T, P> perechi[20];
    typedef pereche<T, P>* iterator;
    pereche<T, P>* begin = perechi, * end = perechi + 20;
    int current = 0;

    iterator insert(T first1, P second1)
    {
        if (current > 20)
        {
            std::cout << "FULL -> exceptie" << std::endl;
            return end;
        }
        else if (find(first1) != end) { std::cout << "Element prezent deja" << std::endl; return end; }
        else
        {
            perechi[current].first = first1;
            perechi[current].second = second1;
            current++;
            return perechi + current - 1;
        }
    }
    iterator insert(pereche<T, P> a)
    {

        if (current > 20)
        {
            std::cout << "Full" << std::endl;
            return end;
        }
        else if (find(a.first) != end) { std::cout << "Element prezent deja" << std::endl; return end; }
        else
        {

            perechi[current].first = a.first;
            perechi[current].second = a.second;
            current++;
            return perechi + current - 1;
        }
    }
    iterator find(T cheie)
    {
        for (int i = 0; i < current; i++)
            if (perechi[i].first == cheie)
            {
                return perechi + i;
            }

        return end;
    }
    void printAll()
    {
        iterator it = begin;
        std::cout << "KEY VALUE" << std::endl;
        for (int it = 0; it <= current - 1; it++)
            std::cout << perechi[it].first << " " << perechi[it].second << std::endl;
    }
    void initialize()
    {
        int constant = 420691337;
        for (iterator it = begin; it != end; it++)
            it->first = static_cast <T> (constant);
    }
    P& operator[](T index)
    {
        harta_stl<T, P>::iterator iter = find(index);
        if (iter == end) iter = insert(index, static_cast<P>(-1));
        return iter->second;
    }
    harta_stl()
    {
        initialize();
    }


};
//TESTING
/*
int main()
{
    harta_stl<int,float>ah;
    harta_stl<int,float>::iterator it;
    ah.insert(4, 6444.5f);
    ah.insert(pereche<int,float>(5, 62.5f));
    ah.insert(6, 6.5f);
    ah.insert(7, 6.5f);
    ah.insert(2, 1);
    ah[3]=43;
    std::cout<<ah[3];
}
*/