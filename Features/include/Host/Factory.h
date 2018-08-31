#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>

#include <functional>

namespace P1 {
  namespace Host {

    template <typename T>
    class ICreator 
    {
    public:
      ~ICreator() {}

      virtual T* create() = 0;
    };

    template <typename ResultInterface, typename T>
    class Creator : public ICreator<ResultInterface>
    {
      typedef std::function< void(T*) > Initializer;

    public:
      Creator(Initializer initializer) 
        : _initializer(initializer)
      {
      }

      ResultInterface* create() override
      {
        T* result = new T();
        if (this->_initializer)
          this->_initializer(result);

        return result;
      }

    private:
      Initializer _initializer;
    };

    template<typename ResultInterface, typename KeyType>
    class Factory
    {
    public:
      Factory() 
      {
      }

      ~Factory() 
      {
        Q_FOREACH(ICreator<ResultInterface>* creator, this->_types)
          delete creator;
      }

      ResultInterface *create(const KeyType& id)
      {
        if (this->_types.find(id) == this->_types.cend())
          return nullptr;

        return this->_types[id]->create();
      }

      template<typename T> 
      void reg(std::function<void(T*)> init = nullptr) 
      {
        if (this->_types.find(T::id()) != this->_types.end())
          return;

        Creator<ResultInterface, T> *c = new Creator<ResultInterface, T>(init);

        this->_types[T::id()] = c;
      }

    private:
      QHash<KeyType, ICreator<ResultInterface> *> _types;
    };

  }
}