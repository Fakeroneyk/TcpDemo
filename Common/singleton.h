//
// Created by kaiyu on 2020/10/27.
//

#ifndef DEMO_SINGLETON_H
#define DEMO_SINGLETON_H

template <typename T>
    class Singleton {
        public:
            static T& Instance() {
                if (NULL == si_instance) {
                    si_instance = new T;
                }
                return *si_instance;
            }

        protected:
            Singleton(){};

        private:
            Singleton(const Singleton &);
            Singleton& operator=(const Singleton &);

            static T *si_instance;
    };

    template <typename T>
    T* Singleton<T>::si_instance = NULL;

#endif //DEMO_SINGLETON_H
