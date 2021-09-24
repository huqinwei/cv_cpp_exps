#ifndef _SIMPLYGONSDK_H_
#define _SIMPLYGONSDK_H_

#include <string.h>

#include <stdint.h>

/// The version of this header, as a macro definition, for conditional builds.
/// Use GetHeaderVersion() to get full version including 
/// build number, and use GetInterfaceVersionHash() to get the hash of the interface.
#define SIMPLYGONSDK_VERSION 0x0803

#ifdef SGDEPRECATED
#undef SGDEPRECATED
#endif
#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif

#if defined(SGSDK) || defined(WITH_SIMPLYGON_DLL) || defined(SGDEPRECATED_OFF)
// internal Simplygon, Unreal Engine, or deprecation warning disabled
#define SGDEPRECATED 
#else
#ifdef _WIN32 // Microsoft compiler
#if (_MSC_VER >= 1300) // 7.1
#define SGDEPRECATED __declspec(deprecated)
#else
#define SGDEPRECATED
#endif
#else
#define SGDEPRECATED
#endif
#endif
	
namespace SimplygonSDK
{

    /** 
	 * CountedPointer is a template class for pointers to API objects. All API interface classes have typedefs for counted pointers that points at the specific class. 
	 * All such typedefs are prefixed with 'sp'. An example of this is 'spObject', which is a typedef for CountedPointer<IObject>. The CountedPointer can be used
	 * for any object that supports reference counting, and exports the AddRef and Release methods. See the IObject class for an example.
	 */
    template <class T> class CountedPointer
		{
		public:
			/**
			 * Constructs a CountedPointer from a standard pointer. If the source pointer points at an API object, a reference to the API object is added.
			 * @param p is a pointer to an API object, or NULL to make the CountedPointer point at nothing.
			 */
			CountedPointer( T *p=NULL ) : ptr(p)
				{
				addref_ptr();
				}
				
			/**
			 * Constructs a CountedPointer from another CountedPointer. If the source pointer points at an API object, a reference to the API object is added.
			 * @param p is a CountedPointer that points at an API object.
			 */
			CountedPointer( const CountedPointer<T> &p ) : ptr(p.ptr)
				{
				addref_ptr();
				}
				
			/**
			 * Destructs the CountedPointer, and releases one reference to the API object, if the CountedPointer does currently point at an object.
			 */
			~CountedPointer()
				{
				release_ptr();
				}
		
			/** 
			 * Tells whether the CountedPointer points at an object, or nothing.
			 * @return true if the pointer points at nothing, of false if the pointer points at an object
			 */
			bool IsNull() const
				{
				return (ptr == NULL);
				}
				
			/**
			 * Sets the pointer to point at the same object as another CountedPointer
			 * @param p is the source CountedPointer object
			 * @return a referece to this object
			 */
			CountedPointer<T>& operator=( const CountedPointer<T> &p ) 
				{
				if( this != &p ) 
					{
					release_ptr();
					ptr = p.ptr;
					addref_ptr();
					}
				return *this;
				}

			/**
			 * Operator that returns the reference of the object the pointer is pointing at. Note! Do not use this operator if the pointer is not pointing at an object. 
			 * @return a referece to the object the pointer is pointing at
			 */
		    T& operator*() const 
				{
		        return *ptr;
				}
				
			/**
			 * Operator that returns a standard pointer to the object the pointer is pointing at. 
			 * @return a pointer to the object the pointer is pointing at
			 */
		    T* operator->() const 
				{
		        return ptr;
				}
								
			/**
			 * Operator that returns a standard pointer to the object the pointer is pointing at. 
			 * @return a pointer to the object the pointer is pointing at
			 */
			operator T* () const
				{
				return ptr;
				}
				
			/**
			 * Method that returns a standard pointer to the object the pointer is pointing at. 
			 * @return a pointer to the object the pointer is pointing at
			 */
			T* GetPointer() const
				{
				return ptr;
				}
				
		private:
			void addref_ptr()
				{
				if( ptr!=NULL )
					{
					ptr->AddRef();
					}
				}
		
			void release_ptr()
				{
				if( ptr!=NULL )
					{
					ptr->Release();
					ptr = NULL;
					}
				}
		
			T *ptr;	
	    };

	class IObject;
	
	/** 
	 * rhandle are used to represent opaque handles returned from and used by the API
	 */
	typedef void* rhandle;
	
	/** 
	 * rid represents ids within the API
	 */
	typedef int rid;

	/**
	 * real represents generic floating point values, depending on API floating point
	 * precision version, reals are either typedef:ed as floats or doubles
	 */
	typedef float real;
			
	const real REAL_MIN = 1.175494351e-38F;
	const real REAL_MAX = 3.402823466e+38F;

	/** 
	 * The robserver class is used to receive events from an interface, by creating a class that
	 * implements the Execute method. By adding an object of the class into an interface using 
	 * the AddObserver method, the Execute method will receive all events of the specified EventId
	 */
	class robserver
		{
		public:
			/** 
			 * The Execute method will receive all events that the robserver is registered to receive.
			 * @param subject is the object that is sending the event
			 * @param EventId is the id of the sent event
			 * @param EventParameterBlock points at a event-specific parameter data block
			 * @param EventParameterBlockSize is the size of the event-specific data block, 
			 * which is useful if the Execute method passes the information on to another handler. Please
			 * note that if the block data is copied to another handler, the data may have to be copied
			 * back into the original block to be able to pass data back to the API class. This is 
			 * however event-specific.
			 */
			virtual void Execute( IObject *subject , rid EventId , void *EventParameterBlock , unsigned int EventParameterBlockSize ) = 0;	
		};

	/**
	 * The rerrorhandler class is used to receive error messages from the interface, by by creating a class that
	 * implements the HandleError method. By setting an object of the class into the main interface using 
	 * the SetErrorHandler method, the HandleError method will receive all error messages from the interface.
	 * Please note that errors may not be recoverable, and could leave objects in an intermediate state. Errors
	 * should in most cases be considered fatal, and will not occur i standard use. 
	 */
	class rerrorhandler
		{
		public:
			/** 
			 * The HandleError method will receive all error messages from the interface.
			 * @param object is the interface object that raised the error message
			 * @param interfacename is the name of the interface 
			 * @param methodname is the name of the method in the object that was called when the error occurred
			 * @param errortype is the type of error that has occurred
			 * @param errortext is a description of the error that has occurred
			 */
			virtual void HandleError( IObject *object , const char *interfacename , const char *methodname , rid errortype , const char *errortext ) = 0;
		};

	/**
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 */
	class IData
		{
		public:
			virtual void AddRef() = 0;
			virtual void Release() = 0;
			virtual void _SetItemCount( unsigned int size ) = 0;
			virtual unsigned int _GetItemCount() const = 0;
            virtual void *_GetRawPointer() = 0;
		};

	/**
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData object, as this will manage reference counting automatically.
	 */
	template<class I> class rdata 
		{
		public:
			typedef typename I::value_type value_type;

            /**
			 * Constructs a rdata from an IData interface pointer. 
			 * @param p is a pointer to an API object, or NULL to make the CountedPointer point at nothing.
			 */
			rdata( I *p=NULL ) : ptr(p)
				{
				addref_ptr();
				}
				
			/**
			 * Points an rdata at content of another rdata. Adds reference count ot the IData object
			 * @param p is a CountedPointer that points at an API object.
			 */
			rdata( const rdata<I> &p ) : ptr(p.ptr)
				{
				addref_ptr();
				}
				
			/**
			 * Destructs the rdata, and releases one reference to the IData object, if the rdata currently points at an object.
			 */
			~rdata()
				{
				release_ptr();
				}
		
			/** 
			 * Tells whether the rdata points at an object, or nothing.
			 * @return true if the pointer points at nothing, of false if the pointer points at an object
			 */
			bool IsNullOrEmpty() const
				{
                if( ptr == nullptr )
                    return true;
                if( ptr->_GetItemCount() == 0)
                    return true;
                return false;
                }
						
			/** 
			 * Operator that sets the rdata to point at the same rdata as the source rdata
			 */
			rdata<I>& operator=( const rdata<I> &p ) 
				{
				if( this != &p ) 
					{
					release_ptr();
					ptr = p.ptr;
					addref_ptr();
					}
				return *this;
				}

			/** 
			 * Method to retrieve the const data pointer. 
			 */
			const value_type *Data() const
				{
                if( this->IsNullOrEmpty() )
                    return nullptr;
				return this->ptr->_GetData();
				}

			/** 
			 * Operator to retrieve the data.
			 */
			operator const value_type *() const
				{
				if( this->IsNullOrEmpty() )
                    return nullptr;
				return this->ptr->_GetData();
				}

			/** 
			 * Method to retrieve the data size.
			 */
			unsigned int GetItemCount() const
				{
                if( this->IsNullOrEmpty() )
                    return 0;
				return this->ptr->_GetItemCount();
				}

			/** 
			 * Method to retrieve a data item value. Note that this will not check the pointer for null 
             * refecence and will cause a crash unless IsNullOrEmpty() is used to check the value of ptr
			 */
			const value_type& GetItem( rid index ) const
				{
				return this->ptr->_GetItem(index);
				}

			/** 
			 * Operator to retrieve a data item value. Note that this will not check the pointer for null 
             * refecence and will cause a crash unless IsNullOrEmpty() is used to check the value of ptr
			 */
			const value_type& operator [] ( rid index ) const
				{
				return this->GetItem( index );
				}
        					
			/**
			 * Operator that returns a standard pointer to the object the pointer is pointing at. 
			 * @return a pointer to the object the pointer is pointing at
			 */
			operator I* () const
				{
				return ptr;
				}
	
			/**
			 * Method that returns a standard pointer to the object the pointer is pointing at. 
			 * @return a pointer to the object the pointer is pointing at
			 */
			I* GetPointer() const
				{
				return ptr;
				}

			/**
			 * Operator that returns a standard pointer to the object the pointer is pointing at. 
			 * @return a pointer to the object the pointer is pointing at
			 */
		    I* operator->() const 
				{
		        return ptr;
				}

        private:
			void addref_ptr()
				{
				if( ptr!=NULL )
					{
					ptr->AddRef();
					}
				}
		
			void release_ptr()
				{
				if( ptr!=NULL )
					{
					ptr->Release();
					ptr = NULL;
					}
				}
		
			I *ptr;	
		};


	/**
	 * The IString class is used to represent strings that are created by the API objects, and returned to the caller.
	 * The strings are reference counted and constant strings, and can be retrieved, but not modified. All
	 * methods that input a string value does this using basic null-terminated char-pointers.
	 * For simplicity it is recommended to never use the IString interface directly, but instead through an 
	 * rstring object, which inherits from a CountedPointer of IString, and that also adds help code and conversions.
	 */
	class IString
		{
		protected:
			char *Text; // the text of the string (warning: never modify this pointer directly)

		public:
            IString() { this->Text = nullptr; }

			/**
			* Never used directly, use rstring objects and functions instead.
			*/
			virtual void AddRef() = 0;
			/**
			* Never used directly, use rstring objects and functions instead.
			*/
			virtual void Release() = 0;
			/**
			* Never used directly, use rstring objects and functions instead.
			*/
			virtual const char *_GetText() const = 0;
			/**
			* Never used directly, use rstring objects and functions instead.
			*/
			virtual bool _IsEmpty() const = 0;
		};
	
	/**
	 * The rstring is a help class that handles type conversion and reference counting for the IString 
	 * interface.
	 * The IString class is used to represent strings that are created by the API objects, and returned to the caller.
	 * The strings are reference counted and constant strings, and can be retrieved, but not modified. All
	 * methods that input a string value does this using basic null-terminated char-pointers.
	 * For simplicity it is recommended to never use the IString interface directly, but instead through an 
	 * rstring object, which inherits from a CountedPointer of IString, and that also adds help code and conversions.
	 */
	class rstring 
		{
		public:
           /**
			 * Constructs a rstring from an IData interface pointer. 
			 * @param p is a pointer to an IString object, or nullptr to make the rstring point at nothing.
			 */
			rstring( IString *p=nullptr ) : ptr(p)
				{
				addref_ptr();
				}
				
			/**
			 * Points an rstring at content of another rstring. Adds reference count ot the IString object
			 * @param p is an rstring that points at an IString object or nullptr
			 */
			rstring( const rstring &p ) : ptr(p.ptr)
				{
				addref_ptr();
				}
				
			/**
			 * Destructs the rdata, and releases one reference to the IData object, if the rdata currently points at an object.
			 */
			~rstring()
				{
				release_ptr();
				}
								
			/** 
			 * Operator that sets the rstring to point at the same IString as the source rstring
			 */
			rstring& operator=( const rstring &p ) 
				{
				if( this != &p ) 
					{
					release_ptr();
					ptr = p.ptr;
					addref_ptr();
					}
				return *this;
				}

			/** 
			 * Method to retrieve the string value as a const char *. If the rstring does not point at an object, NULL is returned
			 */
			const char *GetText() const
				{
                if( ptr == nullptr )
                    return nullptr;
                return ptr->_GetText();
				}

			/** 
			 * Operator to cast the rstring into a const char *. If the rstring does not point at an object, NULL is returned
			 */
			operator const char *() const
				{
				return this->GetText();
				}
				
			/**
			 * Method to check if the rstring is empty, which is the case if the rstring does not point at an IString object,
			 * the IString object does not contain a string, or the string exists, but has zero length.
			 */
			bool IsEmpty() const 
				{
				if( ptr == nullptr )
                    return true;
				return ptr->_IsEmpty();
				}

			/**
			 * Method to check if the rstring pointer is nullptr.
			 */
			bool IsNull() const 
				{
				if( ptr == nullptr )
                    return true;
				return false;
				}
	       
            /**
			 * Operator that returns a standard pointer to the IString that rstring is referencing. Please use IsNull to 
             * check if the pointer is nullptr before using.
			 * @return a pointer to the object the pointer is pointing at
			 */
		    IString* operator->() const 
				{
		        return ptr;
				}

        private:
			void addref_ptr()
				{
				if( ptr!=NULL )
					{
					ptr->AddRef();
					}
				}
		
			void release_ptr()
				{
				if( ptr!=NULL )
					{
					ptr->Release();
					ptr = NULL;
					}
				}
		
			IString *ptr;				
		};
	/** IBoolData is the bool implementation of IData. Do not use directly, instead use spBoolData. */
	class IBoolData : public IData
		{
		protected:
			bool *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef bool value_type;
			IBoolData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const bool *_GetData() const = 0;
			virtual const bool & _GetItem( rid item_index ) const = 0;
		};
															
	/** spBoolData is the alias for rdata<IBoolData,bool>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IBoolData> spBoolData;

	/** ICharData is the char implementation of IData. Do not use directly, instead use spCharData. */
	class ICharData : public IData
		{
		protected:
			char *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef char value_type;
			ICharData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const char *_GetData() const = 0;
			virtual const char & _GetItem( rid item_index ) const = 0;
		};
															
	/** spCharData is the alias for rdata<ICharData,char>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<ICharData> spCharData;

	/** IUnsignedCharData is the unsigned char implementation of IData. Do not use directly, instead use spUnsignedCharData. */
	class IUnsignedCharData : public IData
		{
		protected:
			unsigned char *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef unsigned char value_type;
			IUnsignedCharData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const unsigned char *_GetData() const = 0;
			virtual const unsigned char & _GetItem( rid item_index ) const = 0;
		};
															
	/** spUnsignedCharData is the alias for rdata<IUnsignedCharData,unsigned char>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IUnsignedCharData> spUnsignedCharData;

	/** IShortData is the short implementation of IData. Do not use directly, instead use spShortData. */
	class IShortData : public IData
		{
		protected:
			short *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef short value_type;
			IShortData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const short *_GetData() const = 0;
			virtual const short & _GetItem( rid item_index ) const = 0;
		};
															
	/** spShortData is the alias for rdata<IShortData,short>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IShortData> spShortData;

	/** IUnsignedShortData is the unsigned short implementation of IData. Do not use directly, instead use spUnsignedShortData. */
	class IUnsignedShortData : public IData
		{
		protected:
			unsigned short *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef unsigned short value_type;
			IUnsignedShortData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const unsigned short *_GetData() const = 0;
			virtual const unsigned short & _GetItem( rid item_index ) const = 0;
		};
															
	/** spUnsignedShortData is the alias for rdata<IUnsignedShortData,unsigned short>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IUnsignedShortData> spUnsignedShortData;

	/** IIntData is the int implementation of IData. Do not use directly, instead use spIntData. */
	class IIntData : public IData
		{
		protected:
			int *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef int value_type;
			IIntData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const int *_GetData() const = 0;
			virtual const int & _GetItem( rid item_index ) const = 0;
		};
															
	/** spIntData is the alias for rdata<IIntData,int>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IIntData> spIntData;

	/** IUnsignedIntData is the unsigned int implementation of IData. Do not use directly, instead use spUnsignedIntData. */
	class IUnsignedIntData : public IData
		{
		protected:
			unsigned int *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef unsigned int value_type;
			IUnsignedIntData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const unsigned int *_GetData() const = 0;
			virtual const unsigned int & _GetItem( rid item_index ) const = 0;
		};
															
	/** spUnsignedIntData is the alias for rdata<IUnsignedIntData,unsigned int>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IUnsignedIntData> spUnsignedIntData;

	/** ILongData is the long implementation of IData. Do not use directly, instead use spLongData. */
	class ILongData : public IData
		{
		protected:
			long *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef long value_type;
			ILongData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const long *_GetData() const = 0;
			virtual const long & _GetItem( rid item_index ) const = 0;
		};
															
	/** spLongData is the alias for rdata<ILongData,long>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<ILongData> spLongData;

	/** IUnsignedLongData is the unsigned long implementation of IData. Do not use directly, instead use spUnsignedLongData. */
	class IUnsignedLongData : public IData
		{
		protected:
			unsigned long *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef unsigned long value_type;
			IUnsignedLongData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const unsigned long *_GetData() const = 0;
			virtual const unsigned long & _GetItem( rid item_index ) const = 0;
		};
															
	/** spUnsignedLongData is the alias for rdata<IUnsignedLongData,unsigned long>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IUnsignedLongData> spUnsignedLongData;

	/** IFloatData is the float implementation of IData. Do not use directly, instead use spFloatData. */
	class IFloatData : public IData
		{
		protected:
			float *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef float value_type;
			IFloatData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const float *_GetData() const = 0;
			virtual const float & _GetItem( rid item_index ) const = 0;
		};
															
	/** spFloatData is the alias for rdata<IFloatData,float>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IFloatData> spFloatData;

	/** IDoubleData is the double implementation of IData. Do not use directly, instead use spDoubleData. */
	class IDoubleData : public IData
		{
		protected:
			double *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef double value_type;
			IDoubleData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const double *_GetData() const = 0;
			virtual const double & _GetItem( rid item_index ) const = 0;
		};
															
	/** spDoubleData is the alias for rdata<IDoubleData,double>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IDoubleData> spDoubleData;

	/** IRealData is the real implementation of IData. Do not use directly, instead use spRealData. */
	class IRealData : public IData
		{
		protected:
			real *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef real value_type;
			IRealData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const real *_GetData() const = 0;
			virtual const real & _GetItem( rid item_index ) const = 0;
		};
															
	/** spRealData is the alias for rdata<IRealData,real>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IRealData> spRealData;

	/** IRidData is the rid implementation of IData. Do not use directly, instead use spRidData. */
	class IRidData : public IData
		{
		protected:
			rid *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef rid value_type;
			IRidData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const rid *_GetData() const = 0;
			virtual const rid & _GetItem( rid item_index ) const = 0;
		};
															
	/** spRidData is the alias for rdata<IRidData,rid>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IRidData> spRidData;

	/** IRhandleData is the rhandle implementation of IData. Do not use directly, instead use spRhandleData. */
	class IRhandleData : public IData
		{
		protected:
			rhandle *Data; // the data pointer. only use the pointer for debugging purposes
			unsigned int DataCount; // the number of items. only use the pointer for debugging purposes
			unsigned int DataAllocation; // the number of allocated items. only use the pointer for debugging purposes
														
			int RefCount; // the reference count. only use for debugging purposes
		
		public:
			typedef rhandle value_type;
			IRhandleData()
			{
			    this->Data = nullptr;
			    this->DataCount = 0;
			    this->DataAllocation = 0;
			    this->RefCount = 0;
			}
	   
			virtual const rhandle *_GetData() const = 0;
			virtual const rhandle & _GetItem( rid item_index ) const = 0;
		};
															
	/** spRhandleData is the alias for rdata<IRhandleData,rhandle>
	 * The IData class is used to represent data that is created by the API objects, and returned to the caller.
	 * The data arrays are created in the same address space as the caller application, and the data can be accessed directly.
	 * For ease of use, it is recommended to never use the IData interface directly, but instead through an 
	 * sp[...]Data (e.g. spBoolData, spRidData etc) object, which inherits from a CountedPointer of IData, 
	 * and that also adds help code and conversions.
	 * The rdata class is used to retrieve array data from the interface. Use this rather than the
	 * IData objects, as this is based on a CountedPointer of the interface and adds help methods.	
	 */
	typedef rdata<IRhandleData> spRhandleData;


	/// BaseTypes are ids of the base types used by data containers (arrays etc.) within Simplygon.
	enum BaseTypes
	{
		TYPES_ID_BOOL = 1,
		TYPES_ID_CHAR = 2,
		TYPES_ID_UCHAR = 3,
		TYPES_ID_SHORT = 4,
		TYPES_ID_USHORT = 5,
		TYPES_ID_INT = 6,
		TYPES_ID_UINT = 7,
		TYPES_ID_LONG = 8,
		TYPES_ID_ULONG = 9,
		TYPES_ID_FLOAT = 10,
		TYPES_ID_DOUBLE = 11,
		TYPES_ID_REAL = 12,
		TYPES_ID_RID = 13,
		TYPES_ID_RCHAR = 14, // note: TYPES_ID_RCHAR is deprecated and not used anymore
		TYPES_ID_RHANDLE = 15,
		TYPES_ID_STRING = 16,
		TYPES_ID_OBJECT = 17
	};

	/// EventCodes are used by the event system to specify what kind of event to listen for
	enum EventCodes
	{
		/// updates the current progress, in percent
		SG_EVENT_PROGRESS = 0x01000001,

		/// The evaluate event is triggered from the custom shading node
		SG_EVENT_PROCESS_STARTED = 0x01000002,

		/// The evaluate event is triggered from the custom shading node
		SG_EVENT_CUSTOM_NODE_EVALUATE = 0x00000001,

		/// The generate shader code event is triggered from the custom shading node
		SG_EVENT_CUSTOM_NODE_GENERATE_SHADER_CODE = 0x00000002
	};

	/// Error codes that are returned on initialization/finalization of the library
	enum ErrorCodes
	{
		/// no error
		SG_ERROR_NOERROR = 0,

		/// no license was found (or licence expired)
		SG_ERROR_NOLICENSE = -1,

		/// the SDK is not initialized, or no process object has been loaded/created
		SG_ERROR_NOTINITIALIZED = -2,

		/// the SDK is already initialized
		SG_ERROR_ALREADYINITIALIZED = -3,

		/// the specified file was not found
		SG_ERROR_FILENOTFOUND = -4,

		/// an invalid parameter was passed to the method
		SG_ERROR_INVALIDPARAM = -5,

		/// the reduction failed post-testing
		SG_ERROR_FAILEDTESTING = -6,

		/// the Simplygon DLL and header file interface versions do not match
		SG_ERROR_WRONGVERSION = -7,

		/// the Simplygon DLL failed loading, probably because of a missing dependency
		SG_ERROR_LOADFAILED = -8,

		/// the license is not made for this application of Simplygon
		SG_ERROR_LICENSE_WRONGAPPLICATION = -9,

		/// the license is not made for this platform of Simplygon
		SG_ERROR_LICENSE_WRONGPLATFORM = -10,

		/// the license is not made for this version of Simplygon
		SG_ERROR_LICENSE_WRONGVERSION = -11,

		/// cannot reach the licensing server, cant look up server, check DNS
		SG_ERROR_FAILEDLOOKUP = -10001,

		/// cannot contact the licensing server, check firewall/proxy server
		SG_ERROR_FAILEDCONTACT = -10002,

		/// Failed to connect to the license server. Please check that your internet connection is working and outgoing connections to license.simplygon.com port 443 are not blocked by a firewall or proxy.
		SG_ERROR_FAILEDSEND = -10003,

		/// cannot receive data from the licensing server, check firewall/proxy server
		SG_ERROR_FAILEDRCV = -10004,

		/// data from licensing server is corrupted, try again, check connection
		SG_ERROR_CORRUPTED = -10005,

		/// the license has expired
		SG_ERROR_EXPIRED = -10006,

		/// the license data is corrupted, please reinstall the license key
		SG_ERROR_INVALIDLICENSE = -10007,

		/// the license is not for this product, please contact licensing, and replace license key
		SG_ERROR_WRONGLICENSE = -10008,

		/// no network card was found on the machine
		SG_ERROR_NONWCARD = -10009,

		/// could not decode license, it is corrupted
		SG_ERROR_DECODEFAILED = -10010,

		/// the license is locked to another machine
		SG_ERROR_WRONGMACHINE = -10011,

		/// could not decode license, it is corrupted
		SG_ERROR_DECODEFAILED2 = -10012,

		/// could not decode license, it is corrupted
		SG_ERROR_DECODEFAILED3 = -10013,

		/// the license is invalid, please contact licensing
		SG_ERROR_INVALIDLICENSE2 = -10014,

		/// the license has expired
		SG_ERROR_EXPIRED2 = -10015,

		/// the license data is corrupted, please reinstall the license key
		SG_ERROR_INVALID_LICENSE2 = -10016,

		/// the license is not for this product, please contact licensing, and replace license key
		SG_ERROR_WRONG_LICENSE2 = -10017,

		/// no network card was found on the machine
		SG_ERROR_NO_NWCARD = -10018,

		/// could not decode license, it is corrupted
		SG_ERROR_DECODE_FAILED = -10019,

		/// the license is locked to another machine
		SG_ERROR_WRONG_MACHINE2 = -10020,

		/// could not decode license, it is corrupted
		SG_ERROR_DECODEFAILED4 = -10021,

		/// could not decode license, it is corrupted
		SG_ERROR_DECODEFAILED5 = -10022,

		/// the license is invalid, please contact licensing
		SG_ERROR_INVALIDLICENSE3 = -10023,

		/// your license requires latest Windows 10 version
		SG_ERROR_INVALIDWINDOWSVERSION = -10024,

		/// Bad license request. Please reach out to SimplygonSupport@microsoft.com for support
		SG_ERROR_LICENSE_BADREQUEST = -10400,

		/// You are using an unknown license key. Please verify so you typed in the license key correctly and try again
		SG_ERROR_LICENSE_UNKNOWNLICENSE = -10404,

		/// You are using an inactive license key. Please reach out to SimplygonSales@microsoft.com for support
		SG_ERROR_LICENSE_INACTIVE = -10410,

		/// You have reached the maximum number of nodes for your license. After 48 hours of not using Simplygon on a machine the license will be released and can be installed on another machine
		SG_ERROR_LICENSE_NODECONFLICT = -10409,

		/// The license key you are using requires you to update to the latest version before you can use Simplygon
		SG_ERROR_LICENSE_UPGRADEREQUIRED = -10426,

		/// Bad license request. Please reach out to SimplygonSupport@microsoft.com for support
		SG_ERROR_LICENSE_BADREQUEST2 = -10501
	};

	/// GeometryValidationCodes are used by the Geometry Validation system to report errors in a setup geometry.
	enum GeometryValidationCodes
	{
		/// no error
		SG_VALIDATIONERROR_NOERROR = 0x00000000,

		/// a float is not a number
		SG_VALIDATIONERROR_NAN_FLOAT = 0x00000001,

		/// the triangle count is illegal (probably 0)
		SG_VALIDATIONERROR_ILLEGAL_TRIANGLECOUNT = 0x00000002,

		/// the vertex count is illegal (probably 0)
		SG_VALIDATIONERROR_ILLEGAL_VERTEXCOUNT = 0x00000004,

		/// the vertex indices are illegal
		SG_VALIDATIONERROR_ILLEGAL_INDICES = 0x00000008,

		/// a found triangle has illegal index setup
		SG_VALIDATIONERROR_ILLEGAL_TRIANGLE = 0x00000010,

		/// a field has illegal number of items
		SG_VALIDATIONERROR_ILLEGAL_FIELD_ITEMCOUNT = 0x00000020,

		/// a field has illegal number of tuples
		SG_VALIDATIONERROR_ILLEGAL_FIELD_TUPLECOUNT = 0x00000040,

		/// a field has illegal size of tuples
		SG_VALIDATIONERROR_ILLEGAL_FIELD_TUPLESIZE = 0x00000080,

		/// a field has illegal values (probably an index out of bounds)
		SG_VALIDATIONERROR_ILLEGAL_FIELD_ITEMVALUE = 0x00000100,

		/// a found triangle has no area
		SG_VALIDATIONERROR_ZEROAREA_TRIANGLE = 0x00000200,

		/// the skinning field has illegal values (probably an index out of bounds)
		SG_VALIDATIONERROR_ILLEGAL_BONETUPLESIZE = 0x00000400,

		/// there is a missing object in the scene/geometry that should never be missing
		SG_VALIDATIONERROR_MISSING_OBJECT = 0x00000800,

		/// a found bone is illegal within the scene
		SG_VALIDATIONERROR_ILLEGAL_BONE = 0x00001000,

		/// a normal has zero length
		SG_VALIDATIONERROR_ZERO_LENGTH_NORMAL = 0x00002000,

		/// a float is too large for reliable arithmetic
		SG_VALIDATIONERROR_LARGE_FLOAT = 0x00004000,

		/// a float is too small and denormal
		SG_VALIDATIONERROR_DENORMAL_FLOAT = 0x00008000,

		/// a node has an illegal GUID
		SG_VALIDATIONERROR_ILLEGAL_SCENE_NODE_GUID = 0x00010000
	};

	/// ReductionTargets are used to define which types of reduction targets the reducer will use.
	enum ReductionTargets
	{
		/// 
		SG_REDUCTIONTARGET_TRIANGLECOUNT = 0x00000001,

		/// 
		SG_REDUCTIONTARGET_TRIANGLERATIO = 0x00000002,

		/// 
		SG_REDUCTIONTARGET_MAXDEVIATION = 0x00000004,

		/// 
		SG_REDUCTIONTARGET_ONSCREENSIZE = 0x00000008,

		/// 
		SG_REDUCTIONTARGET_ALL = 0x0000000F
	};

	/// StopCondition determines if the processing will stop when any or all of the set targets have been hit.
	enum StopCondition
	{
		/// Simplygon will stop as soon as one of the reduction targets has been fulfilled.
		SG_STOPCONDITION_ANY = 0x00000000,

		/// Simplygon will continue processing until all of the reduction targets have been fulfilled.
		SG_STOPCONDITION_ALL = 0x00000001
	};

	/// ReductionHeuristics decides how precise calculations Simplygon will do during the optimization of the Scene.
	enum ReductionHeuristics
	{
		/// Faster but with less precise calculations.
		SG_REDUCTIONHEURISTICS_FAST = 0x00000000,

		/// Slower but more accurate.
		SG_REDUCTIONHEURISTICS_CONSISTENT = 0x00000001
	};

	/// DataCreationPreferences specifies how big freedom Simplygon has when it comes to altering vertex/triangle data. Allowing more altered data means it will be easier to create good looking LODs, but without altered data it will be easier to keep the memory-usage in the LOD-chain more optimized.
	enum DataCreationPreferences
	{
		/// Vertex and Corner data such as coordinates and normals will be completely re-used from the original Scene.
		SG_DATACREATIONPREFERENCES_ONLY_USE_ORIGINAL_DATA = 0x00000000,

		/// Vertex coordinates will remaining as they were in the original Scene, but corner normals might be changed if Simplygon decided it will look better.
		SG_DATACREATIONPREFERENCES_PREFER_ORIGINAL_DATA = 0x00000001,

		/// Both vertex coordinates and corner normals will be altered to make the optimized Scene look as good as possible.
		SG_DATACREATIONPREFERENCES_PREFER_OPTIMIZED_RESULT = 0x00000002
	};

	/// This is used when fetching data from the original Scene to the Remeshed scene, for data such as textures, vertex colors and skinning.
	enum RemeshingSurfaceTransferMode
	{
		/// Faster but with less precise calculations.
		SG_SURFACETRANSFER_FAST = 0x00000000,

		/// Slower and more accurate.
		SG_SURFACETRANSFER_ACCURATE = 0x00000001
	};

	/// This is used to toggle between fast and accurate algorithms for different processing steps.
	enum ProcessingMode
	{
		/// Faster but with less precise calculations.
		SG_MODE_FAST = 0x00000000,

		/// Slower and more accurate.
		SG_MODE_ACCURATE = 0x00000001
	};

	/// The fill mode determines what to do with the pixels that remain unfilled after both the casting and dilation has been performed.
	enum AtlasFillMode
	{
		/// linearly interpolate the closest samples
		SG_ATLASFILLMODE_INTERPOLATE = 0x00000000,

		/// use the closest available pixel value without interpolation
		SG_ATLASFILLMODE_NEARESTNEIGHBOR = 0x00000001,

		/// Do not fill remaining pixels
		SG_ATLASFILLMODE_NONE = 0x00000002
	};

	/// BoneReductionTargets are used to define which types of reduction targets the bone reducer will use.
	enum BoneReductionTargets
	{
		/// 
		SG_BONEREDUCTIONTARGET_BONECOUNT = 0x00000001,

		/// 
		SG_BONEREDUCTIONTARGET_BONERATIO = 0x00000002,

		/// 
		SG_BONEREDUCTIONTARGET_MAXDEVIATION = 0x00000004,

		/// 
		SG_BONEREDUCTIONTARGET_ONSCREENSIZE = 0x00000008,

		/// 
		SG_BONEREDUCTIONTARGET_ALL = 0x0000000F
	};

	/// DitherPatterns are used to set the dithering type to use when generating images.
	enum DitherPatterns
	{
		/// Use no dithering
		SG_DITHERPATTERNS_NO_DITHER = 0x00000000,

		/// Use Floys-Steinberg dithering
		SG_DITHERPATTERNS_FLOYDSTEINBERG = 0x00000001,

		/// Use Jarvis, Judice, and Ninke dithering
		SG_DITHERPATTERNS_JARVISJUDICENINKE = 0x00000002,

		/// Use Sierra-3 (three row) dithering
		SG_DITHERPATTERNS_SIERRA = 0x00000003
	};

	/// This decides whether an occlusion mesh generated with the corresponding setting is larger or smaller than the input mesh.
	enum OcclusionModes
	{
		/// The output mesh will be an occluder, ie. always smaller than the input mesh.
		SG_OCCLUSIONMODE_OCCLUDER = 0x00000000,

		/// The output mesh will be an occludee, ie. always larger than the input mesh.
		SG_OCCLUSIONMODE_OCCLUDEE = 0x00000001,

		/// The output mesh will be the same size as the input, ie. appropriate for things like off-screen shadows.
		SG_OCCLUSIONMODE_STANDARD = 0x00000002
	};

	/// TangentSpaceMethod is used to specify which tangent space method is usedto generate tangent spaces when processing. Use SG_TANGENTSPACEMETHOD_ORTHONORMAL_LEFTHANDEDto generate tangents that are D3D compatible (left-handed coordinate system).
	enum TangentSpaceMethod
	{
		/// Standard method, creates an orthonormal tangent space (right-handed)
		SG_TANGENTSPACEMETHOD_ORTHONORMAL = 0x00000000,

		/// 3dsMax compatible method, creates tangent space that is compatible with the 3dsMax renderer
		SG_TANGENTSPACEMETHOD_3DSMAX = 0x00000001,

		/// Creates an orthonormal tangent space (left-handed, D3D compatible)
		SG_TANGENTSPACEMETHOD_ORTHONORMAL_LEFTHANDED = 0x00000002
	};

	/// Validation level is used to specify how often validation is done on geometries when debugging integrations.
	enum ValidationFrequency
	{
		/// Normal level, validation will hardly be noticeable at all
		SG_VALIDATIONFREQUENCY_NORMAL = 0x00000000,

		/// Core debugging level, validation will be carried out before each processing step, and increase processing times significantly
		SG_VALIDATIONFREQUENCY_CORE = 0x00000001,

		/// Ultra level, validation will be carried out very often, multiple times per processing step. This will increase processing times with orders of magnitudes.
		SG_VALIDATIONFREQUENCY_ULTRA = 0x00000002
	};

	/// TextureBlendType specifies which type of blend function to use to blend textures. The blend type function will be used while casting channels during material casting.
	enum TextureBlendType
	{
		/// 
		SG_TEXTUREBLEND_REPLACE = 0x00000000,

		/// 
		SG_TEXTUREBLEND_ADD = 0x00000001,

		/// 
		SG_TEXTUREBLEND_SUBTRACT = 0x00000002,

		/// 
		SG_TEXTUREBLEND_MULTIPLY = 0x00000003,

		/// 
		SG_TEXTUREBLEND_ALPHA = 0x00000004,

		/// 
		SG_TEXTUREBLEND_PRE_MULTIPLIED_ALPHA = 0x00000005,

		/// 
		SG_TEXTUREBLEND_OVER = 0x00000006,

		/// 
		SG_TEXTUREBLEND_IN = 0x00000007,

		/// 
		SG_TEXTUREBLEND_OUT = 0x00000008,

		/// 
		SG_TEXTUREBLEND_ADD_W_ALPHA = 0x00000009,

		/// 
		SG_TEXTUREBLEND_SUBTRACT_W_ALPHA = 0x0000000A,

		/// 
		SG_TEXTUREBLEND_MULTIPLY_W_ALPHA = 0x0000000B
	};

	/// TextureWrapMode specifies which wrap mode will be used for the texture sampling in a specific texture node when casting.
	enum TextureWrapMode
	{
		/// 
		SG_TEXTUREWRAPMODE_WRAP = 0x00000000,

		/// 
		SG_TEXTUREWRAPMODE_CLAMP = 0x00000001,

		/// 
		SG_TEXTUREWRAPMODE_MIRROR = 0x00000002
	};

	/// ImageFileType specifies types of image files
	enum ImageFileType
	{
		/// and unknown or unsupported file type
		SG_IMAGEFILETYPE_UNSUPPORTED = -1,

		/// 
		SG_IMAGEFILETYPE_BMP = 0,

		/// 
		SG_IMAGEFILETYPE_DDS = 1,

		/// 
		SG_IMAGEFILETYPE_GIF = 2,

		/// 
		SG_IMAGEFILETYPE_JPEG = 3,

		/// 
		SG_IMAGEFILETYPE_PNG = 4,

		/// 
		SG_IMAGEFILETYPE_TGA = 5,

		/// 
		SG_IMAGEFILETYPE_TIFF = 6,

		/// 
		SG_IMAGEFILETYPE_PSD = 7
	};

	/// TexcoordGeneratorType specifies which method generates the texture coords
	enum TexcoordGeneratorType
	{
		/// Texture coordinates are generated from scratch based on the geometry.
		SG_TEXCOORDGENERATORTYPE_PARAMETERIZER = 0x00000000,

		/// Texture coordinates are generated based on the original texture coordinates.
		SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR = 0x00000001,

		/// Texture coordinates are generated from scratch based on the geometry.
		SG_TEXCOORDGENERATORTYPE_PARAMETERIZER_OLD = 0x00000002,

		/// Texture coordinates are generated from scratch based on the geometry.
		SG_TEXCOORDGENERATORTYPE_PARAMETERIZER_8_1 = 0x00000000
	};

	/// ChartAggregatorMode specifies in which manner the output UV chart size will depend on the input.
	enum ChartAggregatorMode
	{
		/// Aggregated UV charts will be scaled to keep their relative pixel density relative to all other UV charts. The user can still set the size of the output texture maps.
		SG_CHARTAGGREGATORMODE_TEXTURESIZEPROPORTIONS = 0x00000000,

		/// Aggregated UV charts will have their size set based on its actual geometrical size.
		SG_CHARTAGGREGATORMODE_SURFACEAREA = 0x00000001,

		/// The combined atlas will be resized to fit each chart so that all charts retain the same amount of pixels as they originally had. This will override any manually set texture size.
		SG_CHARTAGGREGATORMODE_ORIGINALPIXELDENSITY = 0x00000002,

		/// Aggregated UV charts will have their size set based on its original UV size, disregarding the size of the texture they are used in.
		SG_CHARTAGGREGATORMODE_UVSIZEPROPORTIONS = 0x00000003
	};

	/// FlipFacesAndNormals specifies how faces and normals should be flipped.
	enum FlipFacesAndNormals
	{
		/// Don't flip.
		SG_FLIPFACESANDNORMALS_NONE = 0x00000000,

		/// Automatically detect and fix backfaces per part.
		SG_FLIPFACESANDNORMALS_AUTOMATIC = 0x00000001,

		/// Append inverted winded triangles with inverted normals to the geometry.
		SG_FLIPFACESANDNORMALS_MAKEDOUBLESIDED = 0x00000002,

		/// Invert the winding of the triangles.
		SG_FLIPFACESANDNORMALS_INVERTFACES = 0x00000003,

		/// Invert the vertex normals.
		SG_FLIPFACESANDNORMALS_INVERTNORMALS = 0x00000004,

		/// Invert winding of all triangles and vertex normals.
		SG_FLIPFACESANDNORMALS_INVERTFACESANDNORMALS = 0x00000005
	};

	/// FoliageType specifies the type of foliage in the foliage processor.
	enum FoliageType
	{
		/// Generates a set of billboards that represents the scene.
		SG_FOLIAGETYPE_BILLBOARDS = 0x00000000,

		/// Renders the scene from a number of views and generates an atlas with the images.
		SG_FOLIAGETYPE_FLIPBOOK = 0x00000001
	};

	/// CameraType specifies the camera used when rendering/computing visibility.
	enum CameraType
	{
		/// Perspective cameras have a field of view.
		SG_CAMERATYPE_PERSPECTIVE = 0x00000000,

		/// Orthographic cameras have a size.
		SG_CAMERATYPE_ORTHOGRAPHIC = 0x00000001,

		/// Omnidirectional cameras view all directions.
		SG_CAMERATYPE_OMNIDIRECTIONAL = 0x00000002
	};

	/// ComputeVisibilityMode specifies which type of renderer to use when computing visibility.
	enum ComputeVisibilityMode
	{
		/// Use DirectX when computing visibility.
		SG_COMPUTE_VISIBILITY_MODE_DIRECTX = 0x00000000,

		/// Use a software renderer when computing visibility.
		SG_COMPUTE_VISIBILITY_MODE_SOFTWARE = 0x00000001
	};

	/// MaterialType specifies the type of material. Certain materials might use specialized shaders.
	enum MaterialType
	{
		/// Use the standard shader.
		SG_MATERIALTYPE_STANDARD = 0x00000000,

		/// Use the flipbook shader.
		SG_MATERIALTYPE_FLIPBOOK = 0x00000001,

		/// Use the billboard cloud shader.
		SG_MATERIALTYPE_BILLBOARDCLOUD = 0x00000002
	};

	/// SettingValueType is used to denot which data type a setting expects.
	enum SettingValueType
	{
		/// Invalid settings value type.
		SG_SETTINGVALUETYPE_INVALID = 0x00000000,

		/// Bool settings type.
		SG_SETTINGVALUETYPE_BOOL = 0x00000001,

		/// Int settings type.
		SG_SETTINGVALUETYPE_INT = 0x00000002,

		/// Unsigned int settings type.
		SG_SETTINGVALUETYPE_UINT = 0x00000003,

		/// Double floating point settings type.
		SG_SETTINGVALUETYPE_DOUBLE = 0x00000004,

		/// String settings type.
		SG_SETTINGVALUETYPE_STRING = 0x00000005
	};

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_AMBIENT = "Ambient";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_DIFFUSE = "Diffuse";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_SPECULAR = "Specular";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_SHININESS = "Shininess";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_OPACITY = "Opacity";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_EMISSIVE = "Emissive";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_NORMALS = "Normals";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_GROUPIDS = "GroupIds";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_DISPLACEMENT = "Displacement";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_BASECOLOR = "Basecolor";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_ROUGHNESS = "Roughness";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_METALNESS = "Metalness";

	/// The SG_MATERIAL_CHANNEL_[...] defines the default names of standard material textures in Simplygon.
	static const char * SG_MATERIAL_CHANNEL_OCCLUSION = "Occlusion";

	/// The SG_IMAGEDATA_FORMAT_[...] defines the default names of standard image formats in Simplygon. L is a grayscale light
	static const char * SG_IMAGEDATA_FORMAT_L = "L";

	/// The SG_IMAGEDATA_FORMAT_[...] defines the default names of standard image formats in Simplygon. RGB is 3-component red, green, blue
	static const char * SG_IMAGEDATA_FORMAT_RGB = "RGB";

	/// The SG_IMAGEDATA_FORMAT_[...] defines the default names of standard image formats in Simplygon. RGBA is 4-component red, green, blue, alpha
	static const char * SG_IMAGEDATA_FORMAT_RGBA = "RGBA";

	/// The maximum number of texture channels supported within Simplygon.
	static const unsigned int SG_NUM_SUPPORTED_TEXTURE_CHANNELS = 256;

	/// The maximum number of color channels supported within Simplygon.
	static const unsigned int SG_NUM_SUPPORTED_COLOR_CHANNELS = 256;

	/// The maximum number of bones that can be references by one vertex within Simplygon.
	static const unsigned int SG_NUM_SUPPORTED_BONES_PER_VERTEX = 16;

	/// The maximum number of mapping layers that are supported internally by Simplygon
	static const unsigned int SG_NUM_SUPPORTED_MAPPING_LAYERS = 32;

	/// The maximum number of inputs that can be connected to any custom node within Simplygon
	static const unsigned int SG_NUM_SUPPORTED_CUSTOM_NODE_INPUTS = 16;

	/// The maximum size of shader output that can be generated by a custom node within Simplygon
	static const unsigned int SG_SUPPORTED_CUSTOM_NODE_SHADER_CODE_SIZE = 2048;

	/// The maximum value allowed for any of the reduction importance multipliers/values
	static const float SG_MAXIMUM_IMPORTANCE_VALUE = (float)1000;

	/// The maximum number of input materials that the mapping image caster supports
	static const unsigned int SG_MAXIMUM_MAPPING_IMAGE_INPUT_MATERIAL_COUNT = 1024;

	/// The maximum number of output materials that the mapping image caster supports
	static const unsigned int SG_MAXIMUM_MAPPING_IMAGE_OUTPUT_MATERIAL_COUNT = 1024;

	/// The maximum number of output materials that the parameterizer supports
	static const unsigned int SG_MAXIMUM_PARAMETERIZER_OUTPUT_MATERIAL_COUNT = 1024;

	/// The maximum number of output materials that the chart aggregator supports
	static const unsigned int SG_MAXIMUM_CHART_AGGREGATOR_OUTPUT_MATERIAL_COUNT = 1024;


	/**
	 * IObject is the base class for all normal classes defined within the API. The class adds safe casting into any API object, reference counting support
	 * (best used with the CountedPointer reference counted pointer class), basic names for all objects, and event handling, using observers (using objects
	 * derived from robserver)
	 */
	class IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();
		
		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;
		
		/**
		 * The IsClassA function returns true if the class is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( type && (strcmp("IObject",type)==0) )
				return true;
			return false;
			}
			
		/** 
		* SafeCast makes sure the pointer is of a class that can be cast into IObject, and if this is the case, returns the pointer cast into an IObject 
		* @param ptr is the pointer to be cast into a pointer to an object of type IObject
		* @return a pointer to IObject, if the cast can be made
		*/
		static IObject *SafeCast( IObject *ptr ) { return ptr; }

		/**
		 * PrintInfo prints the contents/info of the object to the log
		 */
		virtual void PrintInfo() = 0;

		/**
		 * Adds one reference to the object.
		 */
		virtual void AddRef() = 0;

		/**
		 * Releases one reference to the object. When all references have been removed, the object is removed from memory.
		 */
		virtual void Release() = 0;
		
		/** 
		 * Sets the name of the object.
		 * @param Name is the new name of the object
		 */
		virtual void SetName( const char *Name ) = 0;
		
		/** 
		 * Gets the name of the object.
		 * @return The name of the object
		 */
		virtual rstring GetName() = 0;
	
		/** 
		 * Adds a user-defined observer object to the interface, that will receive any event with the specified EventId from the interface.
		 * @param Observer is the object that will receive events
		 * @param EventId the event id to listen for
		 * @return the id of the observer within the API object
		 */
		virtual rid AddObserver( robserver *Observer , rid EventId ) = 0;
		
		/** 
		 * Removes a previously added observer object
		 * @param ObserverId the id returned by AddObserver when the observer was added
		 */
		virtual void RemoveObserver( rid ObserverId ) = 0;

		virtual void *GetUserComponentArea( rid userid ) = 0;
	};

	/**
	 * a CounterPointer smart pointer to an IObject
	 */
	typedef CountedPointer<IObject> spObject;

	class IRidArray;

	/**
	 * IArray is the base class for the array objects. The object contains a dynamically 
	 *  allocated list of tuples of the same length, which can be specified before 
	 * the list is filled with values. 
	 */
	class IArray : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IArray",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IArray pointer
		 * @return a pointer to the IArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IArray") )
				return static_cast<IArray*>(ptr);
			return NULL;
			}

		/**
		 * Creates a new array with the same underlying data type, size and tuple settings 
		 * as this array. To also copy the actual data, set copy_data to true. 
		 * @param copy_data If set, duplicate all the data. If cleared, return an empty 
		 * array of the same type. 
		 * @return the new copy of the array 
		 */
		virtual	CountedPointer<IArray> NewCopy( bool copy_data ) = 0;

		/**
		 * Like NewCopy, NewPackedCopy creates an array with the same underlying data 
		 * and tuple settings as the source array. However, NewPackedCopy only copies 
		 * unique tuples, and if the index_array parameter is set, this index array will 
		 * contain the same number of tuples as the source array, and with the ids of 
		 * a specific tuples within the new copy of the array. 
		 * @param index_array the optional array that will receive the indices of the 
		 * remapped tuples. 
		 * @return a new array with the same properties as this array but only with its 
		 * unique values 
		 */
		virtual	CountedPointer<IArray> NewPackedCopy( IRidArray *index_array ) = 0;

		/**
		 * Copies data from another array. The arrays must have the same underlying data 
		 * types. 
		 * @param source the source array to copy from Value cannot be equal to null. 
		 *  
		 */
		virtual	void DeepCopy( IArray *source ) = 0;

		/**
		 * Copies tuples from a source array through an id table. The id table dictates 
		 * the order in which the tuples are to be copied to this array. E.g. If the first 
		 * item in the id table has the value 14, then the tuple with id 14 in the source 
		 * array will be copied to the first tuple in this array. Note: All ids in the 
		 * id table must be valid ids of tuples in the source array. The id table is assumed 
		 * to have a tuple size of 1. The source array and this array must have the same 
		 * underlying data type, and have the same tuple size. The array must be large 
		 * enough to hold the copied data. 
		 * @param source is the array from which to copy tuples Value cannot be equal 
		 * to null. 
		 * @param idtable is the array with the tuple indices to copy Value cannot be 
		 * equal to null. 
		 * @param startId is the first tuple that will receive the copied values 
		 */
		virtual	void IndexedCopy( IArray *source , IRidArray *idtable , rid startId ) = 0;

		/**
		 * Used to set the tuple size. Warning! Only use SetTupleSize before filling the 
		 * list with values, as any added data in the array will be trashed. 
		 * @param newsize the new tuple size Value cannot be less than 1. Value cannot 
		 * be greater than 1024. 
		 */
		virtual	void SetTupleSize( unsigned int newsize ) = 0;

		/**
		 * Used to get the tuple size. 
		 * @return the tuple size 
		 */
		virtual	unsigned int GetTupleSize(  ) = 0;

		/**
		 * Resizes the list of items to the exact count specified. Warning! Do not confuse 
		 *  this count with the count of tuples, they are only the same if TupleSize is 
		 * equal to 1, i.e. when the tuples are exactly one component each. Also note 
		 * that the item count should be a multiple of TupleSize. Note that the total 
		 * number of items cannot exceed INT_MAX. 
		 * @param count the new number of items. Value cannot be greater than INT_MAX. 
		 *  
		 */
		virtual	void SetItemCount( unsigned int count ) = 0;

		/**
		 * Returns the number of items in the array Warning! Do not confuse this count 
		 * with the count of tuples, they are only the same if TupleSize is equal to one, 
		 * i.e. when the tuples are exactly one component each. 
		 * @return the number of items in the array. 
		 */
		virtual	unsigned int GetItemCount(  ) = 0;

		/**
		 * Returns the id of the last item that is in the array. The id returned is equal 
		 * to (ItemCount - 1). Warning! Do not confuse this id with the id of a tuple, 
		 * they are only the same if TupleSize is equal to one, i.e. when the tuples are 
		 * exactly one component each. 
		 * @return the max item id. 
		 */
		virtual	rid GetMaxItemId(  ) = 0;

		/**
		 * Clears the array and sets the tuple count to 0. Tuple size and name remains 
		 * the same though. 
		 */
		virtual	void Clear(  ) = 0;

		/**
		 * Returns true if the list is empty. 
		 * @return true if empty 
		 */
		virtual	bool IsEmpty(  ) = 0;

		/**
		 * Returns the number of tuples in the list. 
		 * @return the tuple count 
		 */
		virtual	unsigned int GetTupleCount(  ) = 0;

		/**
		 * Returns the id of the tuple last in the list. If the array is empty, the value 
		 * is unspecified. Note: GetMaxTupleId() always returns (GetTupleCount()-1) as 
		 * a signed id 
		 * @return the id of the tuple last in the list 
		 */
		virtual	rid GetMaxTupleId(  ) = 0;

		/**
		 * Resizes the list to the specified tuple count. If the list is enlarged, the 
		 * new tuples will contain unspecified data. Note that the item count equals TupleCount 
		 * multiplied by TupleSize and that the total number of items cannot exceed INT_MAX. 
		 * @param tuplecount the new tuple count Value cannot be greater than INT_MAX. 
		 *  
		 */
		virtual	void SetTupleCount( unsigned int tuplecount ) = 0;

		/**
		 * GetBaseType() returns the id of the basic type that the array is based on The 
		 * ids are listed in the BaseTypes enumeration. 
		 * @return the id of the array type 
		 */
		virtual	rid GetBaseType(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AppendArray AppendArray() \endlink instead. Use AppendArray instead 
		 * of this method. 
		 * @param source the array to append to this array. Value cannot be equal to null. 
		 *  
		 */
		virtual	SGDEPRECATED void AppendTuples( IArray *source ) = 0;

		/**
		 * Appends an array to this array. The array must be a descendant of ReValueArray. 
		 *  For best performance, append arrays of the same basic type. 
		 * @param source the array to append to this array Value cannot be equal to null. 
		 *  
		 */
		virtual	void AppendArray( IArray *source ) = 0;

		/**
		 * Extracts tuples from the array. Note that the destination array must have the 
		 * same classtype and have the same tuple size as the source array. (If both arrays 
		 * are decendants of ReValueArray, they can be of different types, but the conversion 
		 * is slow). 
		 * @param dest the array that will store the extracted tuples Value cannot be 
		 * equal to null. 
		 * @param start this array tuple index where the extraction starts Value cannot 
		 * be less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param count the number of tuples to extract Value cannot be greater than INT_MAX. 
		 *  
		 */
		virtual	void ExtractTuples( IArray *dest , rid start , unsigned int count ) = 0;

		/**
		 * CopyTuple copies one tuple in a source array to a tuple in this array. Both 
		 * the dest_id and the source_id must exist in the arrays. This array can be used 
		 * as the source array to copy within the array. 
		 * @param sourceArray the source array to copy from Value cannot be equal to null. 
		 *  
		 * @param dest_id the tuple index in this array to copy into Value cannot be less 
		 * than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param src_id the tuple index in the source array to copy from Value cannot 
		 * be less than 0. 
		 */
		virtual	void CopyTuple( IArray *sourceArray , rid dest_id , rid src_id ) = 0;

		/**
		 * CompareTuples compares a tuple in this array with a tuple in another array 
		 * @param otherArray the other array to compare with Value cannot be equal to 
		 * null. 
		 * @param tuple_id the tuple from this array to use for comparison Value cannot 
		 * be less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param other_tuple_id the tuple from the other array to use for comparison 
		 * Value cannot be less than 0. 
		 * @return 0 if the tuples are identical, 1 if an element in this array tuple 
		 * in ascending order is larger and -1 if smaller 
		 */
		virtual	int CompareTuples( IArray *otherArray , rid tuple_id , rid other_tuple_id ) = 0;

		/**
		 * CopyRange copies a range of tuples from a source array into this array. The 
		 * arrays must have the same tuple size and base type. This array must be resized 
		 * to hold the tuples before copying. The basetypes are listed in SimplygonSDK::BaseTypes. 
		 * @param source_array the source array to copy from 
		 * @param start_dest_id the first destination tuple id 
		 * @param start_src_id the first source tuple id 
		 * @param count the number of tuples to copy 
		 */
		virtual	void CopyRange( IArray *source_array , rid start_dest_id , rid start_src_id , unsigned int count ) = 0;

		/**
		 * Set the AlternativeName, which can be used by users to set an alternative name 
		 * to the array, when used as field data or in a collection. 
		 * @param value the new alternative name 
		 */
		virtual	void SetAlternativeName( const char * value ) = 0;

		/**
		 * Get the AlternativeName, which can be used by users to set an alternative name 
		 * to the array, when used as field data or in a collection. 
		 * @return the alternative name 
		 */
		virtual	rstring GetAlternativeName(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IArray
	 */
	typedef CountedPointer<IArray> spArray;

	/// Conditional build define for the interface IArray
	#define InterfaceDefined_SimplygonSDK_IArray 1

	class IRidArray;
	class IRealArray;

	/**
	 * ReValueArray adds methods to arrays to generically add, set and get tuples, 
	 * through real values. 
	 */
	class IValueArray : public IArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IValueArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IValueArray",type)==0 )
				return true;
			return IArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IValueArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IValueArray pointer
		 * @return a pointer to the IValueArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IValueArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IValueArray") )
				return static_cast<IValueArray*>(ptr);
			return NULL;
			}

		/**
		 * SetRealItem() sets the item at position id. The id is the id of the item, not 
		 * the tuple the item belongs to. SetRealItem() converts the value from real to 
		 * the actual underlying data type. 
		 * @param id the item to set. Value cannot be less than 0. Value cannot be greater 
		 * than the value returned by GetMaxItemId(). 
		 * @param value the real value to set the item to. 
		 */
		virtual	void SetRealItem( rid id , real value ) = 0;

		/**
		 * GetRealItem() retrieves the item at position id. The id is the id of the item, 
		 * not the tuple the item belongs to. GetRealItem() converts the value from the 
		 * actual underlying data type to a real. 
		 * @param id the item to get. Value cannot be less than 0. Value cannot be greater 
		 * than the value returned by GetMaxItemId(). 
		 * @return the converted real value of the item. 
		 */
		virtual	real GetRealItem( rid id ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 *  Works like IndexedCopy(), but uses two consecutive ids in the idtable, and 
		 * a blend value from the blend table. The idtable contains (idtable_cnt*2) indices 
		 * and blendtable contains idtable_cnt blend values. Note! The array must be large 
		 * enough to hold the copied data. 
		 * @param source the source array to combine from Value cannot be equal to null. 
		 *  
		 * @param idtable is the array with the tuple indices to use for combining Value 
		 * cannot be equal to null. 
		 * @param blendtable the array with (alpha) blend values Value cannot be equal 
		 * to null. 
		 * @param startId is the first tuple that will receive the combined values 
		 */
		virtual	SGDEPRECATED void IndexedCombine( IValueArray *source , IRidArray *idtable , IRealArray *blendtable , rid startId ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 *  Works like IndexedCombine(), but with three ids that are combined through 
		 * two blend values in the blendtable. The idtable contains (idtable_cnt*3) indices 
		 * and blendtable contains (idtable_cnt*2) blend values. Note! The array must 
		 * be large enough to hold the copied data. 
		 * @param source the source array to combine from Value cannot be equal to null. 
		 *  
		 * @param idtable is the array with the tuple indices to use for combining Value 
		 * cannot be equal to null. 
		 * @param blendtable the array with (alpha) blend values Value cannot be equal 
		 * to null. 
		 * @param startId is the first tuple that will receive the combined values 
		 */
		virtual	SGDEPRECATED void IndexedCombine3( IValueArray *source , IRidArray *idtable , IRealArray *blendtable , rid startId ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 *  CopyCombineTuples() combines the data from two tuples from a source array 
		 * into a destination tuple in this array. If InterpolateTuples is set to true, 
		 * dest_id will receive a linear interpolation of the values, using the alpha 
		 * value. If InterpolateTuples is set to false, then if alpha is less than 0.5 
		 * the contents of src_id_1 will end up in dest, else the contents of src_id_2 
		 * will be copied. <br> dest = src_1*(1-alpha) + src_2*alpha <br> The source array 
		 * must be of the same type as this array, and have the same number of items in 
		 * each tuple. This array can be used as the source array to copy within the array. 
		 * @param sourceArray the array to copy from Value cannot be equal to null. 
		 * @param dest_id the tuple index to copy to 
		 * @param src_id_1 the first source array tuple to copy from 
		 * @param src_id_2 the second source array tuple to copy from 
		 * @param alpha the interpolation value 
		 */
		virtual	SGDEPRECATED void CopyCombineTuples( IValueArray *sourceArray , rid dest_id , rid src_id_1 , rid src_id_2 , real alpha ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 *  CopyCombine3Tuples() combines the data from three tuples into a destination 
		 * tuple. The call works like CopyCombineTuples(), but there is three source tuples, 
		 * and two alpha values. the destination will be weighted by the values: <br> 
		 * alpha_3 = 1-(alpha_1 + alpha_2) <br> dest = src_1*alpha_1 + src_2*alpha_2 + 
		 * src_3*alpha_3 <br> The source array must be of the same type as this array, 
		 * and have the same number of items in each tuple. This array can be used as 
		 * the source array to copy within the array. 
		 * @param sourceArray the array to copy from Value cannot be equal to null. 
		 * @param dest_id the tuple index to copy to 
		 * @param src_id_1 the first source array tuple to copy from 
		 * @param src_id_2 the second source array tuple to copy from 
		 * @param src_id_3 the third source array tuple to copy from 
		 * @param alpha_1 the first interpolation value 
		 * @param alpha_2 the second interpolation value 
		 */
		virtual	SGDEPRECATED void CopyCombine3Tuples( IValueArray *sourceArray , rid dest_id , rid src_id_1 , rid src_id_2 , rid src_id_3 , real alpha_1 , real alpha_2 ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 *  Set the interpolation flag. If set, the tuple values will be interpolated 
		 * by calls to CopyCombineTuples, CopyCombine3Tuples, IndexedCombine and IndexedCombine3. 
		 * If not set, then the methods will copy data from the tuple with the highest 
		 * alpha blend value. 
		 * @param value set to true if tuples should be interpolated 
		 */
		virtual	SGDEPRECATED void SetInterpolateTuples( bool value ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 *  Set the interpolation flag. If set, the tuple values will be interpolated 
		 * by calls to CopyCombineTuples, CopyCombine3Tuples, IndexedCombine and IndexedCombine3. 
		 * If not set, then the methods will copy data from the tuple with the highest 
		 * alpha blend value. 
		 * @return the interpolation flag 
		 */
		virtual	SGDEPRECATED bool GetInterpolateTuples(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IValueArray
	 */
	typedef CountedPointer<IValueArray> spValueArray;

	/// Conditional build define for the interface IValueArray
	#define InterfaceDefined_SimplygonSDK_IValueArray 1


	/**
	 * @deprecated Interface is marked as deprecated and will be removed in future 
	 * version. Deprecated interface, which will be removed in version 9.0. ReStringArray 
	 * is the string implementation of ReValueArray. See ReValueArray for a description. 
	 * Please note that conversions to/from real values will not work on string arrays. 
	 *  
	 */
	class IStringArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IStringArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IStringArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IStringArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IStringArray pointer
		 * @return a pointer to the IStringArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IStringArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IStringArray") )
				return static_cast<IStringArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items is being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem / SetRealTuple to 
		 * set the items directly. 
		 * @param value the value of the item 
		 */
		virtual	void AddItem( const char * value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item 
		 * @param value the new value of the item 
		 */
		virtual	void SetItem( rid id , const char * value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item 
		 */
		virtual	rstring GetItem( rid id ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IStringArray
	 */
	typedef CountedPointer<IStringArray> spStringArray;

	/// Conditional build define for the interface IStringArray
	#define InterfaceDefined_SimplygonSDK_IStringArray 1


	/**
	 * IBoolArray is the bool implementation of IValueArray. See IValueArray for a 
	 * description. 
	 */
	class IBoolArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IBoolArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IBoolArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IBoolArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IBoolArray pointer
		 * @return a pointer to the IBoolArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IBoolArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IBoolArray") )
				return static_cast<IBoolArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( bool value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , bool value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	bool GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_boolInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const bool *tuple_boolInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_boolInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const bool *tuple_boolInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IBoolData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IBoolData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IBoolData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_boolInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const bool *source_items_boolInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_boolInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const bool *source_items_boolInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IBoolArray
	 */
	typedef CountedPointer<IBoolArray> spBoolArray;

	/// Conditional build define for the interface IBoolArray
	#define InterfaceDefined_SimplygonSDK_IBoolArray 1


	/**
	 * ICharArray is the char implementation of IValueArray. See IValueArray for a 
	 * description. 
	 */
	class ICharArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ICharArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ICharArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ICharArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ICharArray pointer
		 * @return a pointer to the ICharArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ICharArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ICharArray") )
				return static_cast<ICharArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( char value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , char value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	char GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_charInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const char *tuple_charInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_charInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const char *tuple_charInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , ICharData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( ICharData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , ICharData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_charInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const char *source_items_charInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_charInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const char *source_items_charInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ICharArray
	 */
	typedef CountedPointer<ICharArray> spCharArray;

	/// Conditional build define for the interface ICharArray
	#define InterfaceDefined_SimplygonSDK_ICharArray 1


	/**
	 * IDoubleArray is the double implementation of IValueArray. See IValueArray for 
	 * a description. 
	 */
	class IDoubleArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IDoubleArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IDoubleArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IDoubleArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IDoubleArray pointer
		 * @return a pointer to the IDoubleArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IDoubleArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IDoubleArray") )
				return static_cast<IDoubleArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( double value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , double value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	double GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_doubleInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const double *tuple_doubleInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_doubleInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const double *tuple_doubleInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IDoubleData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IDoubleData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IDoubleData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_doubleInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const double *source_items_doubleInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_doubleInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const double *source_items_doubleInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IDoubleArray
	 */
	typedef CountedPointer<IDoubleArray> spDoubleArray;

	/// Conditional build define for the interface IDoubleArray
	#define InterfaceDefined_SimplygonSDK_IDoubleArray 1


	/**
	 * IFloatArray is the float implementation of IValueArray. See IValueArray for 
	 * a description. 
	 */
	class IFloatArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IFloatArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IFloatArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IFloatArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IFloatArray pointer
		 * @return a pointer to the IFloatArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IFloatArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IFloatArray") )
				return static_cast<IFloatArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( float value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , float value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	float GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_floatInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const float *tuple_floatInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_floatInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const float *tuple_floatInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IFloatData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IFloatData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IFloatData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_floatInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const float *source_items_floatInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_floatInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const float *source_items_floatInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IFloatArray
	 */
	typedef CountedPointer<IFloatArray> spFloatArray;

	/// Conditional build define for the interface IFloatArray
	#define InterfaceDefined_SimplygonSDK_IFloatArray 1


	/**
	 * IIntArray is the int implementation of IValueArray. See IValueArray for a description. 
	 *  
	 */
	class IIntArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IIntArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IIntArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IIntArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IIntArray pointer
		 * @return a pointer to the IIntArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IIntArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IIntArray") )
				return static_cast<IIntArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( int value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , int value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	int GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_intInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const int *tuple_intInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_intInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const int *tuple_intInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IIntData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IIntData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IIntData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_intInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const int *source_items_intInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_intInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const int *source_items_intInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IIntArray
	 */
	typedef CountedPointer<IIntArray> spIntArray;

	/// Conditional build define for the interface IIntArray
	#define InterfaceDefined_SimplygonSDK_IIntArray 1


	/**
	 * ILongArray is the long implementation of IValueArray. See IValueArray for a 
	 * description. 
	 */
	class ILongArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ILongArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ILongArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ILongArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ILongArray pointer
		 * @return a pointer to the ILongArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ILongArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ILongArray") )
				return static_cast<ILongArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( long value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , long value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	long GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_longInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const long *tuple_longInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_longInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const long *tuple_longInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , ILongData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( ILongData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , ILongData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_longInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const long *source_items_longInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_longInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const long *source_items_longInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ILongArray
	 */
	typedef CountedPointer<ILongArray> spLongArray;

	/// Conditional build define for the interface ILongArray
	#define InterfaceDefined_SimplygonSDK_ILongArray 1


	/**
	 * IRealArray is the real implementation of IValueArray. See IValueArray for a 
	 * description. 
	 */
	class IRealArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IRealArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IRealArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IRealArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IRealArray pointer
		 * @return a pointer to the IRealArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IRealArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IRealArray") )
				return static_cast<IRealArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( real value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , real value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	real GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_realInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const real *tuple_realInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_realInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const real *tuple_realInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IRealData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IRealData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IRealData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_realInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const real *source_items_realInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_realInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const real *source_items_realInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IRealArray
	 */
	typedef CountedPointer<IRealArray> spRealArray;

	/// Conditional build define for the interface IRealArray
	#define InterfaceDefined_SimplygonSDK_IRealArray 1


	/**
	 * IRidArray is the rid implementation of IValueArray. See IValueArray for a description. 
	 *  
	 */
	class IRidArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IRidArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IRidArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IRidArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IRidArray pointer
		 * @return a pointer to the IRidArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IRidArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IRidArray") )
				return static_cast<IRidArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( rid value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , rid value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	rid GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_ridInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const rid *tuple_ridInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_ridInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const rid *tuple_ridInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IRidData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IRidData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IRidData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_ridInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const rid *source_items_ridInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_ridInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const rid *source_items_ridInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IRidArray
	 */
	typedef CountedPointer<IRidArray> spRidArray;

	/// Conditional build define for the interface IRidArray
	#define InterfaceDefined_SimplygonSDK_IRidArray 1


	/**
	 * IShortArray is the short implementation of IValueArray. See IValueArray for 
	 * a description. 
	 */
	class IShortArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShortArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShortArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShortArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShortArray pointer
		 * @return a pointer to the IShortArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShortArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShortArray") )
				return static_cast<IShortArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( short value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , short value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	short GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_shortInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const short *tuple_shortInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_shortInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const short *tuple_shortInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IShortData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IShortData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IShortData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_shortInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const short *source_items_shortInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_shortInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const short *source_items_shortInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShortArray
	 */
	typedef CountedPointer<IShortArray> spShortArray;

	/// Conditional build define for the interface IShortArray
	#define InterfaceDefined_SimplygonSDK_IShortArray 1


	/**
	 * IUnsignedCharArray is the uchar implementation of IValueArray. See IValueArray 
	 * for a description. 
	 */
	class IUnsignedCharArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IUnsignedCharArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IUnsignedCharArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IUnsignedCharArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IUnsignedCharArray pointer
		 * @return a pointer to the IUnsignedCharArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IUnsignedCharArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IUnsignedCharArray") )
				return static_cast<IUnsignedCharArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( unsigned char value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , unsigned char value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	unsigned char GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_ucharInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const unsigned char *tuple_ucharInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_ucharInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const unsigned char *tuple_ucharInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IUnsignedCharData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IUnsignedCharData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IUnsignedCharData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_ucharInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const unsigned char *source_items_ucharInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_ucharInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const unsigned char *source_items_ucharInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IUnsignedCharArray
	 */
	typedef CountedPointer<IUnsignedCharArray> spUnsignedCharArray;

	/// Conditional build define for the interface IUnsignedCharArray
	#define InterfaceDefined_SimplygonSDK_IUnsignedCharArray 1


	/**
	 * IUnsignedIntArray is the uint implementation of IValueArray. See IValueArray 
	 * for a description. 
	 */
	class IUnsignedIntArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IUnsignedIntArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IUnsignedIntArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IUnsignedIntArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IUnsignedIntArray pointer
		 * @return a pointer to the IUnsignedIntArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IUnsignedIntArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IUnsignedIntArray") )
				return static_cast<IUnsignedIntArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( unsigned int value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , unsigned int value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	unsigned int GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_uintInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const unsigned int *tuple_uintInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_uintInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const unsigned int *tuple_uintInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IUnsignedIntData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IUnsignedIntData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IUnsignedIntData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_uintInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const unsigned int *source_items_uintInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_uintInputDataPtr the array to copy from. Value cannot be 
		 * equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const unsigned int *source_items_uintInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IUnsignedIntArray
	 */
	typedef CountedPointer<IUnsignedIntArray> spUnsignedIntArray;

	/// Conditional build define for the interface IUnsignedIntArray
	#define InterfaceDefined_SimplygonSDK_IUnsignedIntArray 1


	/**
	 * IUnsignedLongArray is the ulong implementation of IValueArray. See IValueArray 
	 * for a description. 
	 */
	class IUnsignedLongArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IUnsignedLongArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IUnsignedLongArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IUnsignedLongArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IUnsignedLongArray pointer
		 * @return a pointer to the IUnsignedLongArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IUnsignedLongArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IUnsignedLongArray") )
				return static_cast<IUnsignedLongArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( unsigned long value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , unsigned long value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	unsigned long GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_ulongInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const unsigned long *tuple_ulongInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_ulongInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const unsigned long *tuple_ulongInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IUnsignedLongData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IUnsignedLongData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IUnsignedLongData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_ulongInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const unsigned long *source_items_ulongInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_ulongInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const unsigned long *source_items_ulongInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IUnsignedLongArray
	 */
	typedef CountedPointer<IUnsignedLongArray> spUnsignedLongArray;

	/// Conditional build define for the interface IUnsignedLongArray
	#define InterfaceDefined_SimplygonSDK_IUnsignedLongArray 1


	/**
	 * IUnsignedShortArray is the ushort implementation of IValueArray. See IValueArray 
	 * for a description. 
	 */
	class IUnsignedShortArray : public IValueArray
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IUnsignedShortArray is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IUnsignedShortArray",type)==0 )
				return true;
			return IValueArray::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IUnsignedShortArray pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IUnsignedShortArray pointer
		 * @return a pointer to the IUnsignedShortArray object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IUnsignedShortArray *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IUnsignedShortArray") )
				return static_cast<IUnsignedShortArray*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the list, allocates memory as needed to enlarge the list. If 
		 * many items are being put into the list, use Resize to set the size of the new 
		 * list (only one reallocation needed) and then use SetItem() / SetTuple() to 
		 * set the items directly. 
		 * @param value the value of the item. 
		 */
		virtual	void AddItem( unsigned short value ) = 0;

		/**
		 * Sets an item in the list. The id of the item is the exact location of the item 
		 * (TupleId * TupleSize + Index of item in tuple) Warning! No range checking is 
		 * done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 * @param value the new value of the item. 
		 */
		virtual	void SetItem( rid id , unsigned short value ) = 0;

		/**
		 * Gets the value of an item in the list. The id of the item is the exact location 
		 *  of the item (TupleId * TupleSize + Index of item in tuple) Warning! No range 
		 * checking is done in release mode. 
		 * @param id the id of the item. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxItemId(). 
		 */
		virtual	unsigned short GetItem( rid id ) = 0;

		/**
		 * Adds a tuple to the list. Reallocates the list if needed. If many items are 
		 * being put into the list, use Resize to set the size of the new list (only one 
		 * reallocation needed) and then use SetItem() / SetRealTuple() to set the items 
		 * directly. 
		 * @param tuple_ushortInputDataPtr the value(s) of the tuple. 
		 */
		virtual	void AddTuple( const unsigned short *tuple_ushortInputDataPtr ) = 0;

		/**
		 * Sets a tuple in the list. The id is the id of the tuple that is being updated. 
		 * Warning! No range checking is done in release mode. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_ushortInputDataPtr the new value(s) of the tuple. 
		 */
		virtual	void SetTuple( rid id , const unsigned short *tuple_ushortInputDataPtr ) = 0;

		/**
		 * Gets a tuple in the list. The id is the id of the tuple that is being fetched. 
		 * Warning! No range checking is done. 
		 * @param id the id of the tuple. Value cannot be less than 0. Value cannot be 
		 * greater than the value returned by GetMaxTupleId(). 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetTuple( rid id , IUnsignedShortData *dest_param ) = 0;

		/**
		 * Returns all tuples in the array as a single item array. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetData( IUnsignedShortData *dest_param ) = 0;

		/**
		 * Returns a range of tuples in the array. 
		 * @param start_tuple_id the first id in the range. Value cannot be less than 
		 * 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 * @param tuple_count the number of tuples in the range. 
		 * @param dest_param pointer to allocated object to receive return value(s) 
		 */
		virtual	void GetDataRange( rid start_tuple_id , unsigned int tuple_count , IUnsignedShortData *dest_param ) = 0;

		/**
		 * Set the array from a range of data values. Note that the array will be resized 
		 * to the size of the items. Note that the source_item_count is the number of 
		 * items to set, NOT the number of tuples. 
		 * @param source_items_ushortInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetData( const unsigned short *source_items_ushortInputDataPtr , unsigned int source_item_count ) = 0;

		/**
		 * Set a part of the array from a range of data values. Note that the array will 
		 * be resized to contain the size of the items. Note that the source_item_count 
		 * is the number of items to set, NOT the number of tuples, but start_tuple_id 
		 * IS the first tuple index to update. 
		 * @param start_tuple_id where to start copy to in this array. Value cannot be 
		 * less than 0. Value cannot be greater than the value returned by GetMaxTupleId(). 
		 *  
		 * @param source_items_ushortInputDataPtr the array to copy from. Value cannot 
		 * be equal to null. 
		 * @param source_item_count the number of items in the array. 
		 */
		virtual	void SetDataRange( rid start_tuple_id , const unsigned short *source_items_ushortInputDataPtr , unsigned int source_item_count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IUnsignedShortArray
	 */
	typedef CountedPointer<IUnsignedShortArray> spUnsignedShortArray;

	/// Conditional build define for the interface IUnsignedShortArray
	#define InterfaceDefined_SimplygonSDK_IUnsignedShortArray 1

	class IImageData;

	/**
	 * ReChunkedImageData holds chunks for ReImageData objects. ReImageData objects 
	 * can be referenced, and swapped in/out of memory, to secondary storage. This 
	 * is useful for very large images that cannot be stored in-memory. ReImageData 
	 * objects can be indexed in 1-,2- or 3D, and each ReImageData chunk is cloned 
	 * from a source ReImageData object, at setup. Fields added to ReImageData chunks 
	 * after setup will only be added into that chunk, and is not recommended. 
	 */
	class IChunkedImageData : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IChunkedImageData is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IChunkedImageData",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IChunkedImageData pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IChunkedImageData pointer
		 * @return a pointer to the IChunkedImageData object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IChunkedImageData *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IChunkedImageData") )
				return static_cast<IChunkedImageData*>(ptr);
			return NULL;
			}

		/**
		 * Makes a new copy of the chunked image data This image must be fully unlocked 
		 * before copying. 
		 * @param copy_data is set, duplicate the data kept in the image data object 
		 */
		virtual	CountedPointer<IChunkedImageData> NewCopy( bool copy_data ) = 0;

		/**
		 * Deep copies (with template) the data from another image. The source image must 
		 * be fully unlocked before copying. 
		 * @param source source object 
		 * @param copy_data is set, duplicate the data kept in the source image data object 
		 *  
		 */
		virtual	void DeepCopy( IChunkedImageData *source , bool copy_data ) = 0;

		/**
		 * Sets the template image to use as template for creating new chunks. The template 
		 *  image is cloned, it can be released after the setup. Note that SetTemplateImage 
		 *  must be called before allocating image chunks. 
		 * @param _template the template image object 
		 */
		virtual	void SetTemplateImage( IImageData *_template ) = 0;

		/**
		 * Get the X dimension size of the template image used to create the chunks. 
		 * @return the X dimension size of the template image used to create the chunks. 
		 *  
		 */
		virtual	unsigned int GetTemplateXSize(  ) = 0;

		/**
		 * Get the Y dimension size of the template image used to create the chunks. 
		 * @return the Y dimension size of the template image used to create the chunks. 
		 *  
		 */
		virtual	unsigned int GetTemplateYSize(  ) = 0;

		/**
		 * Get the Z dimension size of the template image used to create the chunks. 
		 * @return the Z dimension size of the template image used to create the chunks. 
		 *  
		 */
		virtual	unsigned int GetTemplateZSize(  ) = 0;

		/**
		 * Get the number of image chunks in the X dimension. 
		 * @return the number of image chunks in the X dimension. 
		 */
		virtual	unsigned int GetXSize(  ) = 0;

		/**
		 * Get the number of image chunks in the Y dimension. 
		 * @return the number of image chunks in the Y dimension. 
		 */
		virtual	unsigned int GetYSize(  ) = 0;

		/**
		 * Get the number of image chunks in the Z dimension. 
		 * @return the number of image chunks in the Z dimension. 
		 */
		virtual	unsigned int GetZSize(  ) = 0;

		/**
		 * Set the number of image chunks in the X dimension. 
		 * @param _xs the desired number of chunks in the X dimension. 
		 */
		virtual	void Set1DSize( unsigned int _xs ) = 0;

		/**
		 * Set the number of image chunks in the X and Y dimensions. 
		 * @param _xs the desired number of chunks in the X dimension. 
		 * @param _ys the desired number of chunks in the X dimension. 
		 */
		virtual	void Set2DSize( unsigned int _xs , unsigned int _ys ) = 0;

		/**
		 * Set the number of image chunks in the X, Y and Z dimensions. 
		 * @param _xs the desired number of chunks in the X dimension. 
		 * @param _ys the desired number of chunks in the X dimension. 
		 * @param _zs the desired number of chunks in the X dimension. 
		 */
		virtual	void Set3DSize( unsigned int _xs , unsigned int _ys , unsigned int _zs ) = 0;

		/**
		 * Get the total X dimension size of the full image. 
		 * @return the total X dimension size of the full image. 
		 */
		virtual	unsigned int GetTotalXSize(  ) = 0;

		/**
		 * Get the total Y dimension size of the full image. 
		 * @return the total Y dimension size of the full image. 
		 */
		virtual	unsigned int GetTotalYSize(  ) = 0;

		/**
		 * Get the total Z dimension size of the full image. 
		 * @return the total Z dimension size of the full image. 
		 */
		virtual	unsigned int GetTotalZSize(  ) = 0;

		/**
		 * Clears all data from the image, and removes all data fields. The template image 
		 *  is kept, but can be replaced using SetTemplateImage before calling one of 
		 * the SetXDSize methods. Any lock that was hold in the image is automatically 
		 * released. 
		 */
		virtual	void Reset(  ) = 0;

		/**
		 * Locks a chunk for editing, and returns an image object to the caller. Only 
		 * one lock is allowed per chunk. Use IsChunkLocked to check if a chunk is locked 
		 * before trying to lock it. If the Lock failed, because the chunk is already 
		 * locked, NULL is returned. The method is thread-safe. 
		 * @param x x coordinate of the chunk 
		 */
		virtual	CountedPointer<IImageData> LockChunk1D( rid x ) = 0;

		/**
		 * Locks a chunk for editing, and returns an image object to the caller. Only 
		 * one lock is allowed per chunk. Use IsChunkLocked to check if a chunk is locked 
		 * before trying to lock it. If the Lock failed, because the chunk is already 
		 * locked, NULL is returned. The method is thread-safe. 
		 * @param x x coordinate of the chunk 
		 * @param y y coordinate of the chunk 
		 */
		virtual	CountedPointer<IImageData> LockChunk2D( rid x , rid y ) = 0;

		/**
		 * Locks a chunk for editing, and returns an image object to the caller. Only 
		 * one lock is allowed per chunk. Use IsChunkLocked to check if a chunk is locked 
		 * before trying to lock it. If the Lock failed, because the chunk is already 
		 * locked, NULL is returned. The method is thread-safe. 
		 * @param x x coordinate of the chunk 
		 * @param y y coordinate of the chunk 
		 * @param z z coordinate of the chunk 
		 */
		virtual	CountedPointer<IImageData> LockChunk3D( rid x , rid y , rid z ) = 0;

		/**
		 * Returns true if a chunk is locked. The method is thread-safe. 
		 * @param x x coordinate of the chunk 
		 */
		virtual	bool IsChunkLocked1D( rid x ) = 0;

		/**
		 * Returns true if a chunk is locked. The method is thread-safe. 
		 * @param x x coordinate of the chunk 
		 * @param y y coordinate of the chunk 
		 */
		virtual	bool IsChunkLocked2D( rid x , rid y ) = 0;

		/**
		 * Returns true if a chunk is locked. The method is thread-safe. 
		 * @param x x coordinate of the chunk 
		 * @param y y coordinate of the chunk 
		 * @param z z coordinate of the chunk 
		 */
		virtual	bool IsChunkLocked3D( rid x , rid y , rid z ) = 0;

		/**
		 * Unlocks a previously referenced chunk. All references to the object should 
		 * be released. The method is thread-safe. 
		 * @param x x coordinate of the chunk 
		 */
		virtual	void UnlockChunk1D( rid x ) = 0;

		/**
		 * Unlocks a previously referenced chunk. All references to the object should 
		 * be released. The method is thread-safe. 
		 * @param x x coordinate of the chunk 
		 * @param y y coordinate of the chunk 
		 */
		virtual	void UnlockChunk2D( rid x , rid y ) = 0;

		/**
		 * Unlocks a previously referenced chunk. All references to the object should 
		 * be released. The method is thread-safe. 
		 * @param x x coordinate of the chunk 
		 * @param y y coordinate of the chunk 
		 * @param z z coordinate of the chunk 
		 */
		virtual	void UnlockChunk3D( rid x , rid y , rid z ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IChunkedImageData
	 */
	typedef CountedPointer<IChunkedImageData> spChunkedImageData;

	/// Conditional build define for the interface IChunkedImageData
	#define InterfaceDefined_SimplygonSDK_IChunkedImageData 1

	class IGeometryData;
	class IMaterialTable;
	class IScene;

	/**
	 * A Renderer using DirectX 11 that can be used to preview a scene object containing 
	 *  geometry data by rendering it from selected SceneCamera nodes within the given 
	 * scene and then storing the frames to disc. If using a Shading Node Network (having 
	 * an IShadingNode assigned to the IMaterial), then the material can be previewed 
	 * with the generated HLSL shader. 
	 */
	class IDirectXRenderer : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IDirectXRenderer is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IDirectXRenderer",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IDirectXRenderer pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IDirectXRenderer pointer
		 * @return a pointer to the IDirectXRenderer object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IDirectXRenderer *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IDirectXRenderer") )
				return static_cast<IDirectXRenderer*>(ptr);
			return NULL;
			}

		/**
		 * Creates a previewer with the specified window dimensions. 
		 * @param _width the window width 
		 * @param _height the window height 
		 * @return true if the previewer was successfully created 
		 */
		virtual	bool CreatePreviewer( int _width , int _height ) = 0;

		/**
		 * Render from each camera view in the camera target direction specified in the 
		 * selection set with the scene cameras and stores the frame to the file path 
		 * specified. 
		 * @param cameraSelectionSetID the ID of the selection set with the scene cameras 
		 * to render from 
		 * @param file_name_base the desired directory and file name 
		 * @param format picture format (PNG or RAW) 
		 */
		virtual	void RenderAlongCameraPathAndStorePics( int cameraSelectionSetID , const char * file_name_base , const char * format ) = 0;

		/**
		 * Sets the render mode to wire frame 
		 * @param flag true if render mode should be enabled 
		 */
		virtual	void SetRenderWireFrameMode( bool flag ) = 0;

		/**
		 * Loads a geometry data object and its materials into the previewer. 
		 * @param geom the geometry data object to preview 
		 * @param materials the material table of the geometry. Can be NULL if no material 
		 * table exists. 
		 * @return true if the geometry data was successfully loaded 
		 */
		virtual	bool LoadGeometryData( IGeometryData *geom , IMaterialTable *materials ) = 0;

		/**
		 * Loads a scene into the previewer. The material should have a shading network 
		 * attached to it. 
		 * @param _scene the scene to preview. 
		 * @return true if the scene was successfully loaded 
		 */
		virtual	bool LoadGeometryDataWithMaterialShadingNetwork( IScene *_scene ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IDirectXRenderer
	 */
	typedef CountedPointer<IDirectXRenderer> spDirectXRenderer;

	/// Conditional build define for the interface IDirectXRenderer
	#define InterfaceDefined_SimplygonSDK_IDirectXRenderer 1

	class IRidArray;
	class IRealArray;
	class IValueArray;

	/**
	 * IFieldData represents multiple fields of data. Each field is implemented as 
	 * an IValueArray object, that has a unique name, and can have complex components 
	 * (such as scalars, vectors, quaternions, tensors or matrices). All fields are 
	 * assumed to be of the same length. IFieldData can be seen as an array with complex 
	 * tuples, where the component can be of different types. 
	 */
	class IFieldData : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IFieldData is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IFieldData",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IFieldData pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IFieldData pointer
		 * @return a pointer to the IFieldData object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IFieldData *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IFieldData") )
				return static_cast<IFieldData*>(ptr);
			return NULL;
			}

		/**
		 * Creates another field data object with the same field setup. To also copy the 
		 * data to the new object, set copy_data to true. 
		 * @param copy_data true if data should be copied along with the field properties 
		 *  
		 * @return A copy of this field data 
		 */
		virtual	CountedPointer<IFieldData> NewCopy( bool copy_data ) = 0;

		/**
		 * Like NewCopy, NewPackedCopy creates a field data object with the same underlying 
		 *  data and tuple settings as the source field data object. However, NewPackedCopy 
		 *  only copies unique tuples, and if the index_array parameter is set, this index 
		 * array will contain the same number of tuples as the source field data object, 
		 * and with the ids of a specific tuples within the new copy of the field data 
		 * object. 
		 * @param index_array will contain indices to the new packed field data values 
		 *  
		 * @return A packed copy of this field data 
		 */
		virtual	CountedPointer<IFieldData> NewPackedCopy( IRidArray *index_array ) = 0;

		/**
		 * Copies the field setup and data from another object. To only copy the setup, 
		 * set copy_data to false. 
		 * @param source the source array to copy from 
		 * @param copy_data true if the data should be copied along with the field data 
		 * properties 
		 */
		virtual	void DeepCopy( IFieldData *source , bool copy_data ) = 0;

		/**
		 * Compares the field setup of this field data object to another field data object. 
		 *  If the setups are not identical, false is returned. Note! IsSetupIdenticalTo 
		 * will return false even if the same fields exist in both field data objects, 
		 * but are not in the same order. 
		 * @param other The other field data object used for comparison 
		 * @return true if the field data objects are identical 
		 */
		virtual	bool IsSetupIdenticalTo( IFieldData *other ) = 0;

		/**
		 * Copies tuples from a source field data object through an id table. The id table 
		 *  dictates the order in which the tuples are to be copied. E.g. If the first 
		 * item in the id table has the value 14, then the tuple with id 14 in the source 
		 * array will be copied to the first tuple in this array. <br> Note! All ids in 
		 * the id table must be valid ids of tuples in the source field data object. The 
		 * id table is assumed to have a tuple size of 1. The source and this field data 
		 * object must have the same underlaying data fields, of the same type and the 
		 * same tuple sizes. The FieldData must have enough tuples to hold the new data. 
		 * @param source the source field data object to copy from Value cannot be equal 
		 * to null. 
		 * @param idtable array containing the tuple indices to copy Value cannot be equal 
		 * to null. 
		 * @param startId the first tuple to begin copy into 
		 */
		virtual	void IndexedCopy( IFieldData *source , IRidArray *idtable , rid startId ) = 0;

		/**
		 * Copies tuples from a source field data object through an id table. The id table 
		 *  dictates the order in which the tuples are to be copied. E.g. If the first 
		 * item in the id table has the value 14, then the tuple with id 14 in the source 
		 * array will be copied to the first tuple in this array. <br> Note! All ids in 
		 * the id table must be valid ids of tuples in the source field data object. The 
		 * id table is assumed to have a tuple size of 1. The source and this field data 
		 * object must have the same underlaying data fields, of the same type and the 
		 * same tuple sizes. The FieldData must have enough tuples to hold the new data. 
		 * @param source the source field data object to copy from Value cannot be equal 
		 * to null. 
		 * @param idtable_ridInputDataPtr pointer to indices of tuples to copy Value cannot 
		 * be equal to null. 
		 * @param idtable_cnt the number of indices to copy 
		 * @param startId the first tuple to begin copy into 
		 */
		virtual	void IndexedCopy( IFieldData *source , rid *idtable_ridInputDataPtr , unsigned int idtable_cnt , rid startId ) = 0;

		/**
		 * Works like IndexedCopy, but uses two consecutive ids in the idtable, and a 
		 * blend value from the blendtable. The idtable contains (idtable_cnt*2) indices 
		 * and blendtable contains idtable_cnt blend values. <br> Does the blend: dest 
		 * = src1*(1-blend) + src2*blend <br> The FieldData must have enough tuples to 
		 * hold the new data. 
		 * @param source the field data to combine from Value cannot be equal to null. 
		 *  
		 * @param idtable array containing the tuple indices to combine Value cannot be 
		 * equal to null. 
		 * @param blendtable array containing the blend weights Value cannot be equal 
		 * to null. 
		 * @param startId the tuple to begin putting the combinations into 
		 */
		virtual	void IndexedCombine( IFieldData *source , IRidArray *idtable , IRealArray *blendtable , rid startId ) = 0;

		/**
		 * Works like IndexedCopy, but uses two consecutive ids in the idtable, and a 
		 * blend value from the blendtable. The idtable contains (idtable_cnt*2) indices 
		 * and blendtable contains idtable_cnt blend values. <br> Does the blend: dest 
		 * = src1*(1-blend) + src2*blend <br> The FieldData must have enough tuples to 
		 * hold the new data. 
		 * @param source the field data to combine from Value cannot be equal to null. 
		 *  
		 * @param idtable_ridInputDataPtr array containing the tuple indices to combine 
		 * Value cannot be equal to null. 
		 * @param blendtable_realInputDataPtr pointer to the blend weights Value cannot 
		 * be equal to null. 
		 * @param idtable_cnt the number of indices to combine 
		 * @param startId the tuple to begin putting the combinations into 
		 */
		virtual	void IndexedCombine( IFieldData *source , rid *idtable_ridInputDataPtr , real *blendtable_realInputDataPtr , unsigned int idtable_cnt , rid startId ) = 0;

		/**
		 * Works like IndexedCombine(), but with three ids that are combined through two 
		 * blend values in the blendtable. The idtable contains (idtable_cnt*3) indices 
		 * and blendtable contains (idtable_cnt*2) blend values. <br> Does the blend: 
		 * dest = src1*blend1 + src2*blend2 + src3*(1-(blend1+blend2)) <br> The FieldData 
		 * must have enough tuples to hold the new data. 
		 * @param source the field data to combine from Value cannot be equal to null. 
		 *  
		 * @param idtable array containing the tuple indices to combine Value cannot be 
		 * equal to null. 
		 * @param blendtable array containing the blend weights Value cannot be equal 
		 * to null. 
		 * @param startId the tuple to begin putting the combinations into 
		 */
		virtual	void IndexedCombine3( IFieldData *source , IRidArray *idtable , IRealArray *blendtable , rid startId ) = 0;

		/**
		 * Works like IndexedCombine(), but with three ids that are combined through two 
		 * blend values in the blendtable. The idtable contains (idtable_cnt*3) indices 
		 * and blendtable contains (idtable_cnt*2) blend values. <br> Does the blend: 
		 * dest = src1*blend1 + src2*blend2 + src3*(1-(blend1+blend2)) <br> The FieldData 
		 * must have enough tuples to hold the new data. 
		 * @param source the field data to combine from Value cannot be equal to null. 
		 *  
		 * @param idtable_ridInputDataPtr array containing the tuple indices to combine 
		 * Value cannot be equal to null. 
		 * @param blendtable_realInputDataPtr pointer to the blend weights Value cannot 
		 * be equal to null. 
		 * @param idtable_cnt the number of sets of indices to combine 
		 * @param startId the tuple to begin putting the combinations into 
		 */
		virtual	void IndexedCombine3( IFieldData *source , rid *idtable_ridInputDataPtr , real *blendtable_realInputDataPtr , unsigned int idtable_cnt , rid startId ) = 0;

		/**
		 * Clears all tuples from the fields. The fields, however, are not removed. 
		 */
		virtual	void Clear(  ) = 0;

		/**
		 * Clears all fields from the object. Releases all data in the object. 
		 */
		virtual	void RemoveAllFields(  ) = 0;

		/**
		 * Returns 1 if no tuples exist in the field data. 
		 * @return 1 if empty, else 0 
		 */
		virtual	int IsEmpty(  ) = 0;

		/**
		 * Appends another field data object to this object. The fields are allowed to 
		 * be setup differently, but then comes at a significant performance penalty. 
		 * If fields in the other field data object is missing in this object, these will 
		 * be added, and the items in the old tuples will be set to 0. 
		 * @param other the field data object that is appended Value cannot be equal to 
		 * null. 
		 * @param add_missing_fields if set to true, missing fields will be added to this 
		 * field data object. 
		 */
		virtual	void AppendTuples( IFieldData *other , bool add_missing_fields ) = 0;

		/**
		 * Extracts a range of tuples from this object. The receiving object is assumed 
		 * to have exactly the same data fields as this object. 
		 * @param dest the destination object Value cannot be equal to null. 
		 * @param start the id of the first tuple that is extracted 
		 * @param count the number of tuples to extract 
		 */
		virtual	void ExtractTuples( IFieldData *dest , rid start , unsigned int count ) = 0;

		/**
		 * Adds a field. The field must have a unique name set. 
		 * @param field the array to add to the field data 
		 */
		virtual	void AddField( IValueArray *field ) = 0;

		/**
		 * Adds a field based on a base type. Only IValueArray objects are allowed. The 
		 * created and added object is returned. 
		 * @param base_type should be a base type listed in SimplygonSDK::BaseTypes 
		 * @param tuple_size the desired tuple size 
		 * @param name the name of the new field 
		 * @return a pointer to the new field 
		 */
		virtual	CountedPointer<IValueArray> AddBaseTypeField( rid base_type , unsigned int tuple_size , const char * name ) = 0;

		/**
		 * Retrieves a field. If the field was not found, the return is NULL. 
		 * @param name the name of the field to fetch 
		 * @return the field if it exists, otherwise NULL 
		 */
		virtual	CountedPointer<IValueArray> GetField( const char * name ) = 0;

		/**
		 * Retrieves a field from its index in the field data. 
		 * @param id the id of the field 
		 * @return the field if it exists, otherwise NULL 
		 */
		virtual	CountedPointer<IValueArray> GetFieldWithId( rid id ) = 0;

		/**
		 * Removes a field. Removing a field causes the remaining fields to be remapped 
		 * to new ids. Note that the field must exist. If not, the method will return 
		 * error. To remove a field that may not exist, use SafeRemoveField 
		 * @param name the name of the field 
		 */
		virtual	void RemoveField( const char * name ) = 0;

		/**
		 * Removes a field. Removing a field causes the remaining fields to be remapped 
		 * to new ids. 
		 * @param name the name of the field 
		 */
		virtual	void SafeRemoveField( const char * name ) = 0;

		/**
		 * Returns the number of fields in the field data object. The ids of the fields 
		 * range from 0 through (GetFieldCount()-1) 
		 * @return the number of fields in the field data object 
		 */
		virtual	unsigned int GetFieldCount(  ) = 0;

		/**
		 * Returns the id of the field with the highest id. NOTE! If no fields exist in 
		 * the field data object, the return is undefined. 
		 * @return the id of the field with the highest id 
		 */
		virtual	rid GetMaxFieldId(  ) = 0;

		/**
		 * Returns the handle of the first field. If no fields are added to the object, 
		 * NULL is returned. 
		 * @return the handle of the first field 
		 */
		virtual	rhandle GetFirstFieldHandle(  ) = 0;

		/**
		 * Returns the next handle, or NULL if no more fields exist in the object. 
		 * @param hand the current handle 
		 * @return the handle after the parameter handle 
		 */
		virtual	rhandle GetNextFieldHandle( rhandle hand ) = 0;

		/**
		 * Returns the field associated with the specified handle. 
		 * @param hand the handle to the requested field 
		 * @return the requested field 
		 */
		virtual	CountedPointer<IValueArray> GetFieldWithHandle( rhandle hand ) = 0;

		/**
		 * Resizes the list to the specified tuplecount. If the list is enlarged, the 
		 * new tuples will contain unspecified data. 
		 * @param tuplecount the desired tuple count Value cannot be less than 0. Value 
		 * cannot be greater than INT_MAX. 
		 */
		virtual	void SetTupleCount( unsigned int tuplecount ) = 0;

		/**
		 * Adds a number of tuples to the current tuple count. 
		 * @param tuplecount the desired number of tuples to add. Note that the total 
		 * TupleCount cannot exceed INT_MAX. Value cannot be less than 0. Value cannot 
		 * be greater than INT_MAX. 
		 */
		virtual	void AddTupleCount( unsigned int tuplecount ) = 0;

		/**
		 * Returns the number of tuples. 
		 * @return the number of tuples in the field data 
		 */
		virtual	unsigned int GetTupleCount(  ) = 0;

		/**
		 * Returns the id of the last tuple in the array. If the array is empty, the value 
		 *  is undefined. 
		 * @return the id of the last tuple in the array 
		 */
		virtual	rid GetMaxTupleId(  ) = 0;

		/**
		 * CopyTuple() copies one tuple to another. Both the dest_id and the source_id 
		 * must exist in the array. This field data object can be used as the source field 
		 * data object to copy within the object. 
		 * @param source the source field data to copy from 
		 * @param dest_id the tuple index to copy into in this field data 
		 * @param src_id the tuple index to copy from in the source field data 
		 */
		virtual	void CopyTuple( IFieldData *source , rid dest_id , rid src_id ) = 0;

		/**
		 * CopyCombineTuples() combines the data from two tuples into a destination tuple. 
		 *  This field data object can be used as the source field data object to copy 
		 * within the object. 
		 * @param source the field data source to combine from 
		 * @param dest_id the tuple index to put the combined results into in this field 
		 * data 
		 * @param src_id_1 the first tuple index to use for combining from the field data 
		 * source 
		 * @param src_id_2 the second tuple index to use for combining from the field 
		 * data source 
		 * @param alpha the interpolation value used for combining the source tuples 
		 */
		virtual	void CopyCombineTuples( IFieldData *source , rid dest_id , rid src_id_1 , rid src_id_2 , real alpha ) = 0;

		/**
		 * CopyCombine3Tuples combines the data from three tuples into a destination tuple. 
		 *  The call works like CobineTuples, but there is three source tuples, and two 
		 * alpha values. the destination will be weighted by the values: <br> alpha_3 
		 * = 1-(alpha_1 + alpha_2) <br> dest = src_1*alpha_1 + src_2*alpha_2 + src_3*alpha_3 
		 * <br> This field data object can be used as the source field data object to 
		 * copy within the object. 
		 * @param source the field data source to combine from 
		 * @param dest_id the tuple index to put the combined results into in this field 
		 * data 
		 * @param src_id_1 the first tuple index to use for combining from the field data 
		 * source 
		 * @param src_id_2 the second tuple index to use for combining from the field 
		 * data source 
		 * @param src_id_3 the third tuple index to use for combining from the field data 
		 * source 
		 * @param alpha_1 the first interpolation value used for combining the source 
		 * tuples 
		 * @param alpha_2 the second interpolation value used for combining the source 
		 * tuples 
		 */
		virtual	void CopyCombine3Tuples( IFieldData *source , rid dest_id , rid src_id_1 , rid src_id_2 , rid src_id_3 , real alpha_1 , real alpha_2 ) = 0;

		/**
		 * CopyRange() copies a range of tuples from a source field object into this field 
		 *  object. The field objects must have the same field setup. This field object 
		 * must be resized to hold the tuples before copying. 
		 * @param source the source field object to copy from 
		 * @param start_dest_id the first destination tuple id 
		 * @param start_src_id the first source tuple id 
		 * @param count the number of tuples to copy 
		 */
		virtual	void CopyRange( IFieldData *source , rid start_dest_id , rid start_src_id , unsigned int count ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IFieldData
	 */
	typedef CountedPointer<IFieldData> spFieldData;

	/// Conditional build define for the interface IFieldData
	#define InterfaceDefined_SimplygonSDK_IFieldData 1

	class IPackedGeometryData;
	class IRidArray;
	class IRealArray;
	class IBoolArray;
	class IValueArray;
	class IFieldData;
	class IMatrix4x4;

	/**
	 * IGeometryData represents a geometric structure consisting of point data (Vertices) 
	 *  and topological data (Triangles). IGeometryData may represent a whole geometric 
	 *  object, but can also be used by streaming data filters, and in this way, only 
	 * represents a part of the object. <br><br> Different fields in the Vertices and 
	 * Triangles fields data objects will contain point data and topological data. 
	 * The standard naming convention used in the filters/renderers are as follows: 
	 * (Case sensitive naming, other fields may also be present) <br><br> 'Coords' 
	 * <br> Positional coordinates of a vertex, expressed with a 3-component real field 
	 * (XYZ). <br><br> 'TexCoords0' - 'TexCoords255' <br> Texture coordinates of a 
	 * vertex, expressed with a 2 components real field. By convention, the existing 
	 * fields must be sequential, and must start with 'TexCoord0'. <br><br> 'Normals' 
	 * <br> Normal vector for the vertex, expressed with a 3-component real field, 
	 * a normalized (XYZ) vector. <br><br> 'VertexIds' <br> The id of the primitive's 
	 *  current vertex. This field is present even if the vertex data is directly specified 
	 *  in the primitive's data, to specify topology. The field is of type rid. <br><br> 
	 *  'MaterialIds' <br> The material of the primitive. The field is of type rid. 
	 * <br><br> The 'Coords' and 'VertexIds' fields always exist in the object, but 
	 * the other fields are optional. Also, there can exist user fields. 
	 */
	class IGeometryData : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IGeometryData is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IGeometryData",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IGeometryData pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IGeometryData pointer
		 * @return a pointer to the IGeometryData object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IGeometryData *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IGeometryData") )
				return static_cast<IGeometryData*>(ptr);
			return NULL;
			}

		/**
		 * Creates another geometry data object with the same field setup. To also copy 
		 * the data to the new object, set copy_data to true. 
		 * @param copy_data if true, copies the data along with the IGeometryData properties 
		 *  
		 * @return the new geometry data object 
		 */
		virtual	CountedPointer<IGeometryData> NewCopy( bool copy_data ) = 0;

		/**
		 * Creates a "packed" IPackedGeometryData object, where all fields in the Corner 
		 * field data object is moved to the vertex field data object. Please note that 
		 * the vertex field data object will be enlarged to accommodate for data in a 
		 * vertex shared by multiple triangles, where the corner data differs between 
		 * the triangles. The method will fail if a Corner field is named the same as 
		 * an existing Vertices field, please make sure to remove any such field before 
		 * calling the method. 
		 * @return the new IPackedGeometryData object 
		 */
		virtual	CountedPointer<IPackedGeometryData> NewPackedCopy(  ) = 0;

		/**
		 * Copies the field setup and data from another object into this object. To only 
		 * copy the setup, set copy_data to false. 
		 * @param source the IGeometryData object to copy from 
		 * @param copy_data if true, copies the data along with the IGeometryData properties 
		 *  
		 */
		virtual	void DeepCopy( IGeometryData *source , bool copy_data ) = 0;

		/**
		 * Sets the number of vertices in the geometry. 
		 * @param count the desired new vertex count Value cannot be less than 0. Value 
		 * cannot be greater than INT_MAX. 
		 */
		virtual	void SetVertexCount( unsigned int count ) = 0;

		/**
		 * Gets the number of vertices in the geometry. 
		 * @return the vertex count 
		 */
		virtual	unsigned int GetVertexCount(  ) = 0;

		/**
		 * Increases the vertex count by the specified value. 
		 * @param count the number of vertices to add 
		 */
		virtual	void AddVertices( unsigned int count ) = 0;

		/**
		 * Sets the number of triangles in the geometry. The number of corners is always 
		 * set to (TriangleCount*3), so there is no specific method to set the number 
		 * of corners. 
		 * @param count the desired new triangle count Value cannot be less than 0. Value 
		 * cannot be greater than INT_MAX. 
		 */
		virtual	void SetTriangleCount( unsigned int count ) = 0;

		/**
		 * Gets the number of triangles in the geometry. The number of corners is always 
		 * set to (TriangleCount*3), so there is no specific method to set the number 
		 * of corners. 
		 * @return the number of 
		 */
		virtual	unsigned int GetTriangleCount(  ) = 0;

		/**
		 * AddTriangles increases the triangle count by the specified value. 
		 * @param count the number of new triangles to add 
		 */
		virtual	void AddTriangles( unsigned int count ) = 0;

		/**
		 * Each tuple in the "vertex_pairs" array contains the start-vertex and end-vertex 
		 *  of a half-edge in the geometry. If a half-edge is found from a tuple in the 
		 * "vertex_pairs" array, the edge-id of that half-edge will be stored in the edge_ids 
		 * array. Do note: the half-edges are directed, so if you want all half-edges 
		 * that contains 2 vertices, you need to add the tuple twice. O 
		 * @param vertex_pairs is an array with tuplesize 2, which contains start and 
		 * end-vertex of the edges you want to find. 
		 * @param edge_ids will contain all edge-ids that were found from the vertex-pairs. 
		 * The function will replace all previous data in this array. 
		 */
		virtual	void FindEdgeIdsFromVertexPairs( IRidArray *vertex_pairs , IRidArray *edge_ids ) = 0;

		/**
		 * Gets the Coords field in the vertices. (Field name: "Coords", Tuple size: 3 
		 * , Stored as: Vertex attribute) 
		 * @return the array of coords 
		 */
		virtual	CountedPointer<IRealArray> GetCoords(  ) = 0;

		/**
		 * Adds the VertexLocks field for the vertices. If the value for a vertex is true, 
		 *  then the vertex should be locked, and not be removed in the collapse-process. 
		 * (Field name: "VertexLocks", Tuple size: 1 , Stored as: Vertex attribute) 
		 */
		virtual	void AddVertexLocks(  ) = 0;

		/**
		 * Removes the VertexLocks field for the vertices. If the value for a vertex is 
		 * true, then the vertex should be locked, and not be removed in the collapse-process. 
		 * (Field name: "VertexLocks", Tuple size: 1 , Stored as: Vertex attribute) 
		 */
		virtual	void RemoveVertexLocks(  ) = 0;

		/**
		 * Gets the VertexLocks field for the vertices. If the value for a vertex is true, 
		 *  then the vertex should be locked, and not be removed in the collapse-process. 
		 * (Field name: "VertexLocks", Tuple size: 1 , Stored as: Vertex attribute) 
		 */
		virtual	CountedPointer<IBoolArray> GetVertexLocks(  ) = 0;

		/**
		 * Adds the BoneWeights and BoneIds fields in the vertices. The BoneWeights and 
		 * BoneIds fields are added and removed in unison, and should always be the same 
		 * tuple size. (Field names: "BoneWeights" & "BoneIds", Tuple size: varying , 
		 * Stored as: Vertex attribute) 
		 * @param tuplesize the number of bones allowed per vertex 
		 */
		virtual	void AddBoneWeights( unsigned int tuplesize ) = 0;

		/**
		 * Removes the BoneWeights and BoneIds fields in the vertices. The BoneWeights 
		 * and BoneIds fields are added and removed in unison, and should always be the 
		 * same tuple size. (Field names: "BoneWeights" & "BoneIds", Tuple size: varying 
		 * , Stored as: Vertex attribute) 
		 */
		virtual	void RemoveBoneWeights(  ) = 0;

		/**
		 * Gets the BoneWeights field in the vertices. (Field name: "BoneWeights", Tuple 
		 * size: varying , Stored as: Vertex attribute) 
		 * @return the array containing the bone weights 
		 */
		virtual	CountedPointer<IRealArray> GetBoneWeights(  ) = 0;

		/**
		 * Gets the BoneIds fields in the vertices. (Field name: "BoneIds", Tuple size: 
		 * varying , Stored as: Vertex attribute) 
		 * @return the array containing the bone ids, i.e the bones per vertex that affects 
		 * them 
		 */
		virtual	CountedPointer<IRidArray> GetBoneIds(  ) = 0;

		/**
		 * Adds the VertexWeighting field for the vertices. The weights are used to determine 
		 *  how important it is to keep the vertices when the geometry is reduced. 1 means 
		 * the weight won't change the importance, values closer to 0 means it will be 
		 * less important, values > 1 means the vertex is more important and less probable 
		 * to be removed. (Field name: "VertexWeighting", Tuple size: 1 , Stored as: Vertex 
		 * attribute) 
		 */
		virtual	void AddVertexWeighting(  ) = 0;

		/**
		 * Removes the VertexWeighting field for the vertices. The weights are used to 
		 * determine how important it is to keep the vertices when the geometry is reduced. 
		 * 1 means the weight won't change the importance, values closer to 0 means it 
		 * will be less important, values > 1 means the vertex is more important and less 
		 * probable to be removed. (Field name: "VertexWeighting", Tuple size: 1 , Stored 
		 * as: Vertex attribute) 
		 */
		virtual	void RemoveVertexWeighting(  ) = 0;

		/**
		 * Gets the VertexWeighting field for the vertices. The weights are used to determine 
		 *  how important it is to keep the vertices when the geometry is reduced. 1 means 
		 * the weight won't change the importance, values closer to 0 means it will be 
		 * less important, values > 1 means the vertex is more important and less probable 
		 * to be removed. (Field name: "VertexWeighting", Tuple size: 1 , Stored as: Vertex 
		 * attribute) 
		 * @return the array of vertex weights 
		 */
		virtual	CountedPointer<IRealArray> GetVertexWeighting(  ) = 0;

		/**
		 * Adds a user-specified vertex field. To be able to add a field, it must have 
		 * a name that does not conflict with the existing vertex fields. 
		 * @param field the field to add to this IGeometryData object 
		 */
		virtual	void AddUserVertexField( IValueArray *field ) = 0;

		/**
		 * Removes a user-specified vertex field. 
		 * @param name the name of the user vertex field to be removed from the IGeometryData 
		 * object 
		 */
		virtual	void RemoveUserVertexField( const char * name ) = 0;

		/**
		 * Gets a user-specified vertex field. 
		 * @param name the name of the user vertex field to be fetched from the IGeometryData 
		 * object 
		 * @return the requested user-specified vertex field 
		 */
		virtual	CountedPointer<IValueArray> GetUserVertexField( const char * name ) = 0;

		/**
		 * Adds a user-specified vertex field. To be able to add a field, it must have 
		 * a name that does not conflict with the existing vertex fields. 
		 * @param base_type the BaseType of the new user field, data types are listed 
		 * in SimplygonSDK::BaseTypes 
		 * @param name the name of the new user field 
		 * @param tuple_size the tuple size of the new user field, defaults to 1 if not 
		 * specified 
		 * @return pointer to the created array 
		 */
		virtual	CountedPointer<IValueArray> AddBaseTypeUserVertexField( rid base_type , const char * name , unsigned int tuple_size ) = 0;

		/**
		 * Copies all fields in the specified vertex in the source geometry to this geometry's 
		 *  vertex. 
		 * @param source the IGeometryData object to copy from 
		 * @param dest_id the vertex id to replace in this IGeometryData object 
		 * @param src_id the id of the vertex to copy 
		 */
		virtual	void CopyVertex( IGeometryData *source , rid dest_id , rid src_id ) = 0;

		/**
		 * combines the fields of two vertices by linear interpolation into the destination 
		 *  vertex. 
		 * @param source the IGeometryData object to combine from 
		 * @param dest_id the vertex id to replace in this IGeometryData object 
		 * @param src_id_1 the first vertex id to combine from 
		 * @param src_id_2 the second vertex id to combine from 
		 * @param alpha the interpolation value between 0 and 1 
		 */
		virtual	void CopyCombineVertices( IGeometryData *source , rid dest_id , rid src_id_1 , rid src_id_2 , real alpha ) = 0;

		/**
		 * combines the fields of three vertices via barycentric coordinates. <br> alpha_3 
		 *  = 1-(alpha_1 + alpha_2) <br> dest = src_1*alpha_1 + src_2*alpha_2 + src_3*alpha_3 
		 *  <br> 
		 * @param source the IGeometryData object to combine from 
		 * @param dest_id the vertex id to replace in this IGeometryData object 
		 * @param src_id_1 the first vertex id to combine from 
		 * @param src_id_2 the second vertex id to combine from 
		 * @param src_id_3 the third vertex id to combine from 
		 * @param alpha_1 the first interpolation value 
		 * @param alpha_2 the first interpolation value 
		 */
		virtual	void CopyCombine3Vertices( IGeometryData *source , rid dest_id , rid src_id_1 , rid src_id_2 , rid src_id_3 , real alpha_1 , real alpha_2 ) = 0;

		/**
		 * Retrieves the IFieldData object that contains the vertex fields. 
		 * @return the vertex fields 
		 */
		virtual	CountedPointer<IFieldData> GetVertices(  ) = 0;

		/**
		 * Gets the VertexIds field in the corners. 
		 * @return the VertexIds field 
		 */
		virtual	CountedPointer<IRidArray> GetVertexIds(  ) = 0;

		/**
		 * Adds a TexCoords field in the corners. The valid id range of the level parameter 
		 *  is 0-255, which equals fields 'TexCoords0' - 'TexCoords255' <br>(Field name: 
		 * "TexCoords0" - "TexCoords255", Tuple size: 2 , Stored as: Corner attribute) 
		 * @param level the id of the texture field to add 
		 */
		virtual	void AddTexCoords( rid level ) = 0;

		/**
		 * Adds a named TexCoords field in the corners.<br>(Field name: "TexCoords0" - 
		 * "TexCoords255", Tuple size: 2 , Stored as: Corner attribute) 
		 * @param alternative_name the name of the texcoord level i.e 'TexCoord0' 
		 */
		virtual	rid AddNamedTexCoords( const char * alternative_name ) = 0;

		/**
		 * Removes a TexCoords field in the corners. The valid id range of the level parameter 
		 *  is 0-255, which equals fields 'TexCoords0' - 'TexCoords255' <br>(Field name: 
		 * "TexCoords0" - "TexCoords255", Tuple size: 2 , Stored as: Corner attribute) 
		 * @param level the id of the texture field to remove 
		 */
		virtual	void RemoveTexCoords( rid level ) = 0;

		/**
		 * Removes a named TexCoords field in the corners. 'TexCoords0' - 'TexCoords255' 
		 * <br>(Field name: "TexCoords0" - "TexCoords255", Tuple size: 2 , Stored as: 
		 * Corner attribute) 
		 * @param alternative_name the name of the texcoord level i.e 'TexCoord0' 
		 */
		virtual	bool RemoveNamedTexCoords( const char * alternative_name ) = 0;

		/**
		 * Gets a TexCoords field in the corners. The valid id range of the level parameter 
		 *  is 0-255, which equals fields 'TexCoords0' - 'TexCoords255' <br>(Field name: 
		 * "TexCoords0" - "TexCoords255", Tuple size: 2 , Stored as: Corner attribute) 
		 * @param level the id of the texture field to get 
		 * @return the requested field 
		 */
		virtual	CountedPointer<IRealArray> GetTexCoords( rid level ) = 0;

		/**
		 * Gets a TexCoords field, using the alternative name instead of the index 
		 * @param alternative_name the alternative name of the field. 
		 * @return the requested field, if it was found, or null if not 
		 */
		virtual	CountedPointer<IRealArray> GetNamedTexCoords( const char * alternative_name ) = 0;

		/**
		 * Adds Tangents and Bitangents fields in the corners. The valid id range of the 
		 * level parameter is 0-255. <br>(Field names: "Tangents0" - "Tangents255" & "Bitangents0" 
		 *  - "Bitangents255", Tuple size: 3 , Stored as: Corner attribute) 
		 * @param level the texture channel 
		 */
		virtual	void AddTangents( rid level ) = 0;

		/**
		 * Removes Tangents and Bitangents fields in the corners. The valid id range of 
		 * the level parameter is 0-255. <br>(Field names: "Tangents0" - "Tangents255" 
		 * & "Bitangents0" - "Bitangents255", Tuple size: 3 , Stored as: Corner attribute) 
		 * @param level the texture channel 
		 */
		virtual	void RemoveTangents( rid level ) = 0;

		/**
		 * Gets a Tangents field in the corners. The valid id range of the level parameter 
		 *  is 0-255. <br>(Field name: "Tangents0" - "Tangents255", Tuple size: 3 , Stored 
		 * as: Corner attribute) 
		 * @param level the texture channel 
		 * @return the tangent field for the selected texture channel 
		 */
		virtual	CountedPointer<IRealArray> GetTangents( rid level ) = 0;

		/**
		 * Gets a Bitangents field in the corners. The valid id range of the level parameter 
		 *  is 0-255. <br>(Field name: "Bitangents0" - "Bitangents255", Tuple size: 3 
		 * , Stored as: Corner attribute) 
		 * @param level the texture channel 
		 * @return the bi-tangent field for the selected texture channel 
		 */
		virtual	CountedPointer<IRealArray> GetBitangents( rid level ) = 0;

		/**
		 * Adds the Normals field in the corners. <br>(Field name: "Normals" , Tuple size: 
		 *  3 , Stored as: Corner attribute) 
		 */
		virtual	void AddNormals(  ) = 0;

		/**
		 * Removes the Normals field in the corners. <br>(Field name: "Normals" , Tuple 
		 * size: 3 , Stored as: Corner attribute) 
		 */
		virtual	void RemoveNormals(  ) = 0;

		/**
		 * Gets the Normals field in the corners. <br>(Field name: "Normals" , Tuple size: 
		 *  3 , Stored as: Corner attribute) 
		 * @return the normals field 
		 */
		virtual	CountedPointer<IRealArray> GetNormals(  ) = 0;

		/**
		 * Adds a Color field in the corners. The valid id range of the level parameter 
		 * is 0-255, which equals fields 'Colors0' - 'Colors255' <br>(Field name: "Colors0" 
		 * - "Colors255", Tuple size: 4 , Stored as: Corner attribute) 
		 * @param level the color level (0-255) 
		 */
		virtual	void AddColors( rid level ) = 0;

		/**
		 * Adds a named Color field in the corners.<br>(Field name: "Colors0" - "Colors255", 
		 * Tuple size: 4 , Stored as: Corner attribute) 
		 * @param alternative_name the named color channel "Colors0" - "Colors255" 
		 */
		virtual	rid AddNamedColors( const char * alternative_name ) = 0;

		/**
		 * Removes a Color field in the corners. The valid id range of the level parameter 
		 *  is 0-255, which equals fields 'Colors0' - 'Colors255' <br>(Field name: "Colors0" 
		 *  - "Colors255", Tuple size: 4 , Stored as: Corner attribute) 
		 * @param level the color level (0-255) 
		 */
		virtual	void RemoveColors( rid level ) = 0;

		/**
		 * Removes a named Color field in the corners. <br>(Field name: "Colors0" - "Colors255", 
		 * Tuple size: 4 , Stored as: Corner attribute) 
		 * @param alternative_name the named color channel "Colors0" - "Colors255" 
		 */
		virtual	bool RemoveNamedColors( const char * alternative_name ) = 0;

		/**
		 * Gets a Color field in the corners. The valid id range of the level parameter 
		 * is 0-255, which equals fields 'Colors0' - 'Colors255' <br>(Field name: "Colors0" 
		 * - "Colors255", Tuple size: 4 , Stored as: Corner attribute) 
		 * @param level the color level (0-255) 
		 * @return the color field 
		 */
		virtual	CountedPointer<IRealArray> GetColors( rid level ) = 0;

		/**
		 * Gets a Color field, using the alternative name instead of the index 
		 * @param alternative_name the alternative name of the field. 
		 * @return the requested field, if it was found, or null if not 
		 */
		virtual	CountedPointer<IRealArray> GetNamedColors( const char * alternative_name ) = 0;

		/**
		 * Adds a user-specified corner field. To be able to add a field, it must have 
		 * a name that does not conflict with the existing corner fields. 
		 * @param field the field to add into the IGeometryData object 
		 */
		virtual	void AddUserCornerField( IValueArray *field ) = 0;

		/**
		 * Removes a user-specified corner field. 
		 * @param name the name of the field to remove from the IGeometryData object 
		 */
		virtual	void RemoveUserCornerField( const char * name ) = 0;

		/**
		 * Gets a user-specified corner field. 
		 * @param name the name of the field to get 
		 * @return the requested field 
		 */
		virtual	CountedPointer<IValueArray> GetUserCornerField( const char * name ) = 0;

		/**
		 * Adds an empty user-specified corner field. 
		 * @param base_type specifies the base type of the new corner field, data types 
		 * are listed in SimplygonSDK::BaseTypes 
		 * @param name the name of the field to add 
		 * @param tuple_size the tuple size of the new field, defaults to 1 if not specified 
		 *  
		 * @return pointer to the created field 
		 */
		virtual	CountedPointer<IValueArray> AddBaseTypeUserCornerField( rid base_type , const char * name , unsigned int tuple_size ) = 0;

		/**
		 * Copies all fields in the specified corner in the source geometry to this geometry's 
		 *  corner. 
		 * @param source the IGeometryData object to copy from 
		 * @param dest_id the vertex id to replace in this IGeometryData object 
		 * @param src_id the id of the vertex to copy 
		 */
		virtual	void CopyCorner( IGeometryData *source , rid dest_id , rid src_id ) = 0;

		/**
		 * combines the fields of two corners by linear interpolation into the destination 
		 *  corner. 
		 * @param source the IGeometryData object to combine from 
		 * @param dest_id the corner id to replace in this IGeometryData object 
		 * @param src_id_1 the first corner id to combine from 
		 * @param src_id_2 the second corner id to combine from 
		 * @param alpha the interpolation value between 0 and 1 
		 */
		virtual	void CopyCombineCorners( IGeometryData *source , rid dest_id , rid src_id_1 , rid src_id_2 , real alpha ) = 0;

		/**
		 * Combines three corners via barycentric coordinates. <br> alpha_3 = 1-(alpha_1 
		 * + alpha_2) <br> dest = src_1*alpha_1 + src_2*alpha_2 + src_3*alpha_3 <br> 
		 * @param source the IGeometryData object to combine from 
		 * @param dest_id the corner id to replace in this IGeometryData object 
		 * @param src_id_1 the first corner id to combine from 
		 * @param src_id_2 the second corner id to combine from 
		 * @param src_id_3 the third corner id to combine from 
		 * @param alpha_1 the first interpolation value 
		 * @param alpha_2 the second interpolation value 
		 */
		virtual	void CopyCombine3Corners( IGeometryData *source , rid dest_id , rid src_id_1 , rid src_id_2 , rid src_id_3 , real alpha_1 , real alpha_2 ) = 0;

		/**
		 * Retrieves the IFieldData object that contains the corner fields 
		 * @return the corner IFieldData 
		 */
		virtual	CountedPointer<IFieldData> GetCorners(  ) = 0;

		/**
		 * Adds the MaterialIds field in the triangles. (Field name: "MaterialIds", Tuple 
		 * size: 1 , Stored as: triangle attribute) 
		 */
		virtual	void AddMaterialIds(  ) = 0;

		/**
		 * Removes the MaterialIds field in the triangles. (Field name: "MaterialIds", 
		 * Tuple size: 1 , Stored as: triangle attribute) 
		 */
		virtual	void RemoveMaterialIds(  ) = 0;

		/**
		 * Gets the MaterialIds field in the triangles. (Field name: "MaterialIds", Tuple 
		 * size: 1 , Stored as: triangle attribute) 
		 * @return The MaterialIds field 
		 */
		virtual	CountedPointer<IRidArray> GetMaterialIds(  ) = 0;

		/**
		 * Adds the GroupIds field in the triangles. (Field name: "GroupIds", Tuple size: 
		 * 1 , Stored as: triangle attribute) 
		 */
		virtual	void AddGroupIds(  ) = 0;

		/**
		 * Removes the GroupIds field in the triangles. (Field name: "GroupIds", Tuple 
		 * size: 1 , Stored as: triangle attribute) 
		 */
		virtual	void RemoveGroupIds(  ) = 0;

		/**
		 * Gets the GroupIds field in the triangles. (Field name: "GroupIds", Tuple size: 
		 * 1 , Stored as: triangle attribute) 
		 */
		virtual	CountedPointer<IRidArray> GetGroupIds(  ) = 0;

		/**
		 * Adds a user-specified triangle field. To be able to add a field, it must have 
		 * a name that does not conflict with the existing triangle fields. 
		 * @param field the triangle field to add to the IGeometryData object 
		 */
		virtual	void AddUserTriangleField( IValueArray *field ) = 0;

		/**
		 * Removes a user-specified triangle field. 
		 * @param name the name of the triangle field to remove from the IGeometryData 
		 * object 
		 */
		virtual	void RemoveUserTriangleField( const char * name ) = 0;

		/**
		 * Gets a user-specified triangle field. 
		 * @param name the name of the triangle field to get from the IGeometryData object 
		 *  
		 * @return the requested triangle field 
		 */
		virtual	CountedPointer<IValueArray> GetUserTriangleField( const char * name ) = 0;

		/**
		 * Adds an empty user-specified triangle field. 
		 * @param base_type the base_type of the new user triangle field, data types are 
		 * listed in SimplygonSDK::BaseTypes 
		 * @param name the name of the triangle field to add to the IGeometryData object 
		 *  
		 * @param tuple_size the tuple size of the new field, deafaults to 1 if not specified 
		 *  
		 * @return pointer to the newly created triangle field 
		 */
		virtual	CountedPointer<IValueArray> AddBaseTypeUserTriangleField( rid base_type , const char * name , unsigned int tuple_size ) = 0;

		/**
		 * Copies all fields in the specified triangle in the source geometry to this 
		 * geometry's triangle. 
		 * @param source the IGeometryData object to copy from 
		 * @param dest_id the triangle id to replace in this IGeometryData object 
		 * @param src_id the id of the triangle to copy 
		 */
		virtual	void CopyTriangle( IGeometryData *source , rid dest_id , rid src_id ) = 0;

		/**
		 * combines the fields of two triangles by linear interpolation into the destination 
		 *  triangle. 
		 * @param source the IGeometryData object to combine from 
		 * @param dest_id the triangle id to replace in this IGeometryData object 
		 * @param src_id_1 the first triangle id to combine from 
		 * @param src_id_2 the second triangle id to combine from 
		 * @param alpha the interpolation value between 0 and 1 
		 */
		virtual	void CopyCombineTriangles( IGeometryData *source , rid dest_id , rid src_id_1 , rid src_id_2 , real alpha ) = 0;

		/**
		 * combines the fields of three triangles via barycentric coordinates. <br> alpha_3 
		 *  = 1-(alpha_1 + alpha_2) <br> dest = src_1*alpha_1 + src_2*alpha_2 + src_3*alpha_3 
		 *  <br> 
		 * @param source the IGeometryData object to combine from 
		 * @param dest_id the triangle id to replace in this IGeometryData object 
		 * @param src_id_1 the first triangle id to combine from 
		 * @param src_id_2 the second triangle id to combine from 
		 * @param src_id_3 the third triangle id to combine from 
		 * @param alpha_1 the first interpolation value 
		 * @param alpha_2 the second interpolation value 
		 */
		virtual	void CopyCombine3Triangles( IGeometryData *source , rid dest_id , rid src_id_1 , rid src_id_2 , rid src_id_3 , real alpha_1 , real alpha_2 ) = 0;

		/**
		 * Retrieves the IFieldData object that contains the triangle fields 
		 * @return the triangle field data object 
		 */
		virtual	CountedPointer<IFieldData> GetTriangles(  ) = 0;

		/**
		 * Adds a custom field. To be able to add a field, it must have a name that does 
		 * not conflict with the existing custom fields. 
		 * @param field the custom field to add to the IGeometryData object 
		 */
		virtual	void AddCustomField( IValueArray *field ) = 0;

		/**
		 * Removes a custom field. 
		 * @param name the name of the custom field to remove from the IGeometryData object 
		 *  
		 */
		virtual	void RemoveCustomField( const char * name ) = 0;

		/**
		 * Gets a custom field. 
		 * @param name the name of the custom field to get from the IGeometryData object 
		 *  
		 * @return the requested custom field 
		 */
		virtual	CountedPointer<IValueArray> GetCustomField( const char * name ) = 0;

		/**
		 * Adds an empty custom field. To be able to add a field, it must have a name 
		 * that does not conflict with the existing custom fields. 
		 * @param base_type the base_type of the new custom field, data types are listed 
		 * in SimplygonSDK::BaseTypes 
		 * @param name the name of the new custom field 
		 * @param tuple_size the tuple size of the new custom field, defaults to 1 if 
		 * not specified 
		 * @return pointer to the created field 
		 */
		virtual	CountedPointer<IValueArray> AddBaseTypeCustomField( rid base_type , const char * name , unsigned int tuple_size ) = 0;

		/**
		 * Finds all Nan (1.#IND, 1.#INF etc) values in the real/float/double arrays in 
		 * the GeometryData, and sets them to 0.0 just to make sure all numbers in the 
		 * GeometryData are legit. Should not be needed if all the data loaded into the 
		 * GeometryData object is ok to begin with... 
		 */
		virtual	void CleanupNanValues(  ) = 0;

		/**
		 * Finds triangles that contain the same vertex more than once, and invalidates 
		 * them, ie. sets all the vertex id references to -1. These triangles can be culled 
		 * using Compact(). 
		 */
		virtual	void CleanupInvalidTriangles(  ) = 0;

		/**
		 * Returns the level index of the texcoord field with the name returns -1 if not 
		 * found 
		 * @param field_name the named TexCoord field to be fetched 
		 * @return the index of the field 
		 */
		virtual	rid FindNamedTexCoordsIndex( const char * field_name ) = 0;

		/**
		 * Returns the level index of the colors field with the name returns -1 if not 
		 * found 
		 * @param field_name the named Colors field to be fetched 
		 * @return the index of the field 
		 */
		virtual	rid FindNamedColorsIndex( const char * field_name ) = 0;

		/**
		 * Expands the Vertices field data so that there exists one vertex for each of 
		 * the three corners of each triangle in the geometry. The vertices will be organized 
		 * identical to the Corners field. The triangles will be remapped to use the new 
		 * vertices correctly. Expanding the vertex field is required to be able to extract 
		 * a range of the geometry, and to be able to stream it. Note! This will lead 
		 * to increased memory requirement, as vertex data is not shared among neigbouring 
		 * triangles. It also removes the connectivity information of the triangles, and 
		 * effectively detaches all triangles from one another. This call may replace 
		 * fields in the Vertices field data. Requery any previously cached field pointer 
		 * after the call. All user fields are copied as well. 
		 */
		virtual	void ExpandVertices(  ) = 0;

		/**
		 * Extracts a range of triangles with their vertices from the geometry. ExpandVertices() 
		 *  must first be called for the extraction to succeed. ExtractGeometry() can 
		 * be used to stream geometries in chunks. If writing to a stream, the extracted 
		 * geometry is guaranteed to have the same setup as the original. Any previous 
		 * data in the destination will be deleted. 
		 * @param dest the IGeometryData object that is written to 
		 * @param start_tri_id the first triangle to be extracted 
		 * @param count the number of triangles to extract 
		 */
		virtual	void ExtractRange( IGeometryData *dest , rid start_tri_id , unsigned int count ) = 0;

		/**
		 * Extracts specified triangles from the geometry. The destination geometry will 
		 * contain the triangles and vertices. The vertices in the destination will be 
		 * expanded so that there exists one vertex for each of the three corners of each 
		 * triangle in the geometry. 
		 * @param dest the IGeometryData object that is written to 
		 * @param triangle_ids the list of triangles indices to extract 
		 */
		virtual	void ExtractTriangles( IGeometryData *dest , IRidArray *triangle_ids ) = 0;

		/**
		 * Appends the geometry data to this geometry. This geometry will contain all 
		 * triangles and vertices of both geometries. The VertexIds that are copied from 
		 * the source are remapped to point at the vertices, which are appended to the 
		 * end of the current vertices. Any field in the appended geometry that does not 
		 * exist in this geometry will be added, with the tuples that are added set to 
		 * 0. The method will fail if the source geometry has identically named fields, 
		 * but with different tuple sizes or base types. 
		 * @param source the source geometry that will be appended to this geometry. the 
		 * contents of source will not be modified. 
		 */
		virtual	void AppendGeometry( IGeometryData *source ) = 0;

		/**
		 * Removes invalid triangles and vertices not referenced by any triangle. If any 
		 * vertices are removed, the triangles will be remapped to the new compacted vertex 
		 * field. 
		 * @param only_compact_triangles if true, no vertices will be removed. 
		 */
		virtual	void Compact( bool only_compact_triangles ) = 0;

		/**
		 * Removes invalid triangles and vertices not referenced by any triangle. If any 
		 * vertices are removed, the triangles will be remapped to the new compacted vertex 
		 * field. Essentially calls Compact( false ), included for compatibility. 
		 */
		virtual	void Compact(  ) = 0;

		/**
		 * Welds all vertices in the geometry that are closer than the welding threshold. 
		 * Non-referenced vertices will not be removed, only the vertex indices will be 
		 * remapped. Call Compact() to clean up the geometry and free up memory. 
		 * @param threshold the welding threshold distance 
		 */
		virtual	void Weld( real threshold ) = 0;

		/**
		 * Get the inferior (minimum) extent of the geometry 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetInf( real *realReturnDataPtr ) = 0;

		/**
		 * Set the inferior (minimum) extent of the geometry 
		 * @param vec_realInputDataPtr the new inferior 
		 */
		virtual	void SetInf( const real *vec_realInputDataPtr ) = 0;

		/**
		 * Get the superior (maximum) extent of the geometry 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetSup( real *realReturnDataPtr ) = 0;

		/**
		 * Set the superior (maximum) extent of the geometry 
		 * @param vec_realInputDataPtr the new superior 
		 */
		virtual	void SetSup( const real *vec_realInputDataPtr ) = 0;

		/**
		 * Calculate the extents by checking the coordinates of all vertices in the geometry. 
		 *  Set only_triangles to true to test only vertices that are indexed by a triangle. 
		 *  This is a slower test, and the bounding box may not encompass all vertices' 
		 * positions. the new extents are found by calling GetInf() and GetSup() afterwards. 
		 * @param only_triangles true if only vertices indexed by triangles should be 
		 * checked, defaults to false if not specified 
		 */
		virtual	void CalculateExtents( bool only_triangles ) = 0;

		/**
		 * Tests if the extents of the geometry fully contain all vertex coordinates. 
		 * Note! ExtentsContainCoords() checks all vertices in the geometry, regardless 
		 * to whether the vertex is referenced by any triangle. Use Compact() to remove 
		 * any non-referenced vertex before calling ExtentsContainCoords. 
		 * @return true if geometry fully contain all vertex coordinates 
		 */
		virtual	bool ExtentsContainCoords(  ) = 0;

		/**
		 * Transforms the geometry using the supplied matrix transformation. The fields 
		 * that are modified are the Coords field of Vertices, as well as the Normals 
		 * and all Tangent and Bi-tangent fields of the Corners 
		 * @param transformation the 4x4 transformation as a IMatrix4x4 
		 */
		virtual	void Transform( IMatrix4x4 *transformation ) = 0;

		/**
		 * Detect triangle edge neighbors. Neighbor ids are stored in the "SgEdgeNeighbours" 
		 *  Corner field. 
		 */
		virtual	void DetectEdgeNeighbours(  ) = 0;

		/**
		 * Converts the handedness of the triangle winding, coords, normals and texture 
		 * coordinates of the geometry from left-handed to right-handed coordinate systems 
		 * and vice versa. Simplygon generally uses right-handed coordinates, so this 
		 * conversion needs to be done before processing any left-handed geometry 
		 */
		virtual	void ConvertHandedness(  ) = 0;

		/**
		 * Change winding 
		 */
		virtual	void ChangeWinding(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AddBaseTypeUserCornerField AddBaseTypeUserCornerField() \endlink 
		 * instead. AddBaseTypeUserTriangleVertexField() has been renamed AddBaseTypeUserCornerField() 
		 * @param base_type the base_type of the new custom field, data types are listed 
		 * in SimplygonSDK::BaseTypes 
		 * @param name the name of the new custom field 
		 * @param tuple_size the tuple size of the new custom field, defaults to 1 if 
		 * not specified 
		 */
		virtual	SGDEPRECATED CountedPointer<IValueArray> AddBaseTypeUserTriangleVertexField( rid base_type , const char * name , unsigned int tuple_size ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetCorners GetCorners() \endlink instead. GetTriangleVertices() is 
		 * renamed GetCorners() 
		 */
		virtual	SGDEPRECATED CountedPointer<IFieldData> GetTriangleVertices(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link CopyCorner CopyCorner() \endlink instead. CopyTriangleVertex() is 
		 * renamed CopyCorner() CopyCombineTriangleVertices() is renamed CopyCombineCorners() 
		 * CopyCombine3TriangleVertices() is renamed CopyCombine3Corners() 
		 * @param source the IGeometryData object to copy from 
		 * @param dest_id the triangle id to replace in this IGeometryData object 
		 * @param src_id the id of the triangle to copy 
		 */
		virtual	SGDEPRECATED void CopyTriangleVertex( IGeometryData *source , rid dest_id , rid src_id ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link CopyCombineCorners CopyCombineCorners() \endlink instead. CopyTriangleVertex() 
		 * is renamed CopyCorner() CopyCombineTriangleVertices() is renamed CopyCombineCorners() 
		 * CopyCombine3TriangleVertices() is renamed CopyCombine3Corners() 
		 * @param source the IGeometryData object to combine from 
		 * @param dest_id the corner id to replace in this IGeometryData object 
		 * @param src_id_1 the first corner id to combine from 
		 * @param src_id_2 the second corner id to combine from 
		 * @param alpha the interpolation value between 0 and 1 
		 */
		virtual	SGDEPRECATED void CopyCombineTriangleVertices( IGeometryData *source , rid dest_id , rid src_id_1 , rid src_id_2 , real alpha ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link CopyCombine3Corners CopyCombine3Corners() \endlink instead. CopyTriangleVertex() 
		 * is renamed CopyCorner() CopyCombineTriangleVertices() is renamed CopyCombineCorners() 
		 * CopyCombine3TriangleVertices() is renamed CopyCombine3Corners() 
		 * @param source the IGeometryData object to combine from 
		 * @param dest_id the corner id to replace in this IGeometryData object 
		 * @param src_id_1 the first corner id to combine from 
		 * @param src_id_2 the second corner id to combine from 
		 * @param src_id_3 the third corner id to combine from 
		 * @param alpha_1 the first interpolation value 
		 * @param alpha_2 the second interpolation value 
		 */
		virtual	SGDEPRECATED void CopyCombine3TriangleVertices( IGeometryData *source , rid dest_id , rid src_id_1 , rid src_id_2 , rid src_id_3 , real alpha_1 , real alpha_2 ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AddBoneWeights AddBoneWeights() \endlink instead. Function is empty, 
		 * does now nothing. BoneIds are added/removed when AddBoneWeights()/RemoveBoneWeights() 
		 *  is called. 
		 * @param tuplesize the number of bones allowed per vertex. 
		 */
		virtual	SGDEPRECATED void AddBoneIds( unsigned int tuplesize ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RemoveBoneWeights RemoveBoneWeights() \endlink instead. Function 
		 * is empty, does now nothing. BoneIds are added/removed when AddBoneWeights()/RemoveBoneWeights() 
		 *  is called. 
		 */
		virtual	SGDEPRECATED void RemoveBoneIds(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AddUserCornerField AddUserCornerField() \endlink instead. AddUserTriangleVertexField() 
		 * is renamed AddUserCornerField() RemoveUserTriangleVertexField() is renamed 
		 * RemoveUserCornerField() GetUserTriangleVertexField is renamed GetUserCornerField() 
		 * @param field the field to add into the IGeometryData object 
		 */
		virtual	SGDEPRECATED void AddUserTriangleVertexField( IValueArray *field ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RemoveUserCornerField RemoveUserCornerField() \endlink instead. AddUserTriangleVertexField() 
		 * is renamed AddUserCornerField() RemoveUserTriangleVertexField() is renamed 
		 * RemoveUserCornerField() GetUserTriangleVertexField is renamed GetUserCornerField() 
		 * @param name the field to be removed from the IGeometryData object 
		 */
		virtual	SGDEPRECATED void RemoveUserTriangleVertexField( const char * name ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetUserCornerField GetUserCornerField() \endlink instead. AddUserTriangleVertexField() 
		 * is renamed AddUserCornerField() RemoveUserTriangleVertexField() is renamed 
		 * RemoveUserCornerField() GetUserTriangleVertexField is renamed GetUserCornerField() 
		 * @param name the name of the user triangle field to fetch from the IGeometryData 
		 * object 
		 */
		virtual	SGDEPRECATED CountedPointer<IValueArray> GetUserTriangleVertexField( const char * name ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AddColors AddColors() \endlink instead. DiffuseColors is now an alias 
		 * for 'Colors0' 
		 */
		virtual	SGDEPRECATED void AddDiffuseColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RemoveColors RemoveColors() \endlink instead. DiffuseColors is now 
		 * an alias for 'Colors0' 
		 */
		virtual	SGDEPRECATED void RemoveDiffuseColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetColors GetColors() \endlink instead. DiffuseColors is now an alias 
		 * for 'Colors0' 
		 */
		virtual	SGDEPRECATED CountedPointer<IRealArray> GetDiffuseColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AddColors AddColors() \endlink instead. SpecularColors is now an 
		 * alias for 'Colors1' 
		 */
		virtual	SGDEPRECATED void AddSpecularColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RemoveColors RemoveColors() \endlink instead. SpecularColors is now 
		 * an alias for 'Colors1' 
		 */
		virtual	SGDEPRECATED void RemoveSpecularColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetColors GetColors() \endlink instead. SpecularColors is now an 
		 * alias for 'Colors1' 
		 */
		virtual	SGDEPRECATED CountedPointer<IRealArray> GetSpecularColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AddTangents AddTangents() \endlink instead. Bitangents are Added 
		 * and Removed in unison with Tangents. Only need to call AddTangents to add both 
		 * Tangent and Bitangent fields, and RemoveTangents to remove both fields. Use 
		 * GetBitangents to retrieve the bitangent field. 
		 * @param level the level of the new field 
		 */
		virtual	SGDEPRECATED void AddBitangents( rid level ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RemoveTangents RemoveTangents() \endlink instead. Bitangents are 
		 * Added and Removed in unison with Tangents. Only need to call AddTangents to 
		 * add both Tangent and Bitangent fields, and RemoveTangents to remove both fields. 
		 *  Use GetBitangents to retrieve the bitangent field. 
		 * @param level the level of the field to be removed 
		 */
		virtual	SGDEPRECATED void RemoveBitangents( rid level ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IGeometryData
	 */
	typedef CountedPointer<IGeometryData> spGeometryData;

	/// Conditional build define for the interface IGeometryData
	#define InterfaceDefined_SimplygonSDK_IGeometryData 1

	class IGeometryData;
	class IRidArray;
	class IRealArray;
	class IBoolArray;
	class IValueArray;
	class IFieldData;

	/**
	 * IPackedGeometryData keeps the same information as IGeometryData, but with all 
	 * corner data fields moved into per-vertex fields. No per-corner fields exist, 
	 * apart from the VertexIds field. Please note that the number of vertices in an 
	 * IPackedGeometryData is commonly higher than in an IGeometryData, as vertices 
	 * must be split to accommodate for different corner data. 
	 */
	class IPackedGeometryData : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IPackedGeometryData is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IPackedGeometryData",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IPackedGeometryData pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IPackedGeometryData pointer
		 * @return a pointer to the IPackedGeometryData object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IPackedGeometryData *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IPackedGeometryData") )
				return static_cast<IPackedGeometryData*>(ptr);
			return NULL;
			}

		/**
		 * Creates another geometry data object with the same field setup. To also copy 
		 * the data to the new object, set copy_data to true. 
		 * @param copy_data determines if the data is included in the new copy 
		 * @return the new packed geometry data object 
		 */
		virtual	CountedPointer<IPackedGeometryData> NewCopy( bool copy_data ) = 0;

		/**
		 * Creates an "unpacked" ReGeometryData object. Please note that since the data 
		 * in the RePackedGeometryData structure is per-vertex, you may have to weld the 
		 * resulting ReGeometryData to remove vertices with the same spatial coordinates. 
		 * @return the new unpacked geometry data object 
		 */
		virtual	CountedPointer<IGeometryData> NewUnpackedCopy(  ) = 0;

		/**
		 * Copies the field setup and data from another object. To only copy the setup, 
		 * set copy_data to false. 
		 * @param source is the source from which the data will be copied 
		 * @param copy_data is the bool determining whether to copy the data or just the 
		 * field setup 
		 */
		virtual	void DeepCopy( IPackedGeometryData *source , bool copy_data ) = 0;

		/**
		 * Appends all vertex-tuples in "source" that don't already exists in the current 
		 * GeometryData. For each vertex field in this PackedGeometryData, the "source" 
		 * must contain a corresponding vertex or corner field. All extra-fields in "source" 
		 * will be skipped however. Returns the number of new and unique vertices created, 
		 * and the NewVertexIds will contain the indices for the triangles of the "source" 
		 * Geometry. The VertexIds of the current GeometryData will still be valid. If 
		 * collapseToOriginalVertices is set to true, all vertices from source will be 
		 * collapsed to the closest matching vertex of the current geometry. This means 
		 * that no new vertices will be added, so the returned value will always be 0. 
		 * If the "NewVertexIds"-array is not initiated before send into this function, 
		 * it will do nothing and return -1 instead, as error value. The method does not 
		 * append triangle ids or triangle field data from "source" to the current object. 
		 * @param source is the geometry from which to append 
		 * @param NewVertexIds will be written to and contain the indices for the appended 
		 * geometries triangles 
		 * @param collapseToOriginalVertices will determine if the appended vertices are 
		 * collapsed to existing ones or not 
		 * @return the number of unique verts added to the packed geometry 
		 */
		virtual	int AppendPackedGeometry( IGeometryData *source , IRidArray *NewVertexIds , bool collapseToOriginalVertices ) = 0;

		/**
		 * Sets the number of vertices in the geometry. 
		 * @param count is the new vertexcount. Value cannot be less than 0. Value cannot 
		 * be greater than INT_MAX. 
		 */
		virtual	void SetVertexCount( unsigned int count ) = 0;

		/**
		 * Gets the number of vertices in the geometry. 
		 * @return the current vertex count 
		 */
		virtual	unsigned int GetVertexCount(  ) = 0;

		/**
		 * AddVertices increases the vertex count by the specified value. 
		 * @param count is the number of vertices to add 
		 */
		virtual	void AddVertices( unsigned int count ) = 0;

		/**
		 * Sets the number of triangles in the geometry. 
		 * @param count is the number of triangles to add. Value cannot be less than 0. 
		 * Value cannot be greater than INT_MAX. 
		 */
		virtual	void SetTriangleCount( unsigned int count ) = 0;

		/**
		 * Gets the number of triangles in the geometry. 
		 * @return the current triangle count 
		 */
		virtual	unsigned int GetTriangleCount(  ) = 0;

		/**
		 * Increases the triangle count by the specified value. 
		 * @param count is the number of triangles to add 
		 * @return the current triangle count 
		 */
		virtual	void AddTriangles( unsigned int count ) = 0;

		/**
		 * Gets the Coords field in the vertices. <br>(Field name: "Coords", Tuple size: 
		 * 3 , Stored as: Vertex attribute) 
		 * @return the vertex coords array 
		 */
		virtual	CountedPointer<IRealArray> GetCoords(  ) = 0;

		/**
		 * Adds the VertexLocks field for the vertices. If the value for a vertex is true, 
		 *  then the vertex should be locked, and not be removed in the collapse-process. 
		 * <br>(Field name: "VertexLocks", Tuple size: 1 , Stored as: Vertex attribute) 
		 *  
		 */
		virtual	void AddVertexLocks(  ) = 0;

		/**
		 * Removes the VertexLocks field for the vertices. If the value for a vertex is 
		 * true, then the vertex should be locked, and not be removed in the collapse-process. 
		 * <br>(Field name: "VertexLocks", Tuple size: 1 , Stored as: Vertex attribute) 
		 *  
		 */
		virtual	void RemoveVertexLocks(  ) = 0;

		/**
		 * Gets the VertexLocks field for the vertices. If the value for a vertex is true, 
		 *  then the vertex should be locked, and not be removed in the collapse-process. 
		 * <br>(Field name: "VertexLocks", Tuple size: 1 , Stored as: Vertex attribute) 
		 * @return an array containing the vertex lock bools 
		 */
		virtual	CountedPointer<IBoolArray> GetVertexLocks(  ) = 0;

		/**
		 * Adds the BoneWeights and BoneIds fields in the vertices. The BoneWeights and 
		 * BoneIds fields are added and removed in unison, and should always be the same 
		 * tuple size. <br>(Field names: "BoneWeights" & "BoneIds", Tuple size: varying 
		 * , Stored as: Vertex attribute) 
		 * @param tuplesize is the size of the tuples in the new bone weight and bone 
		 * id field 
		 */
		virtual	void AddBoneWeights( unsigned int tuplesize ) = 0;

		/**
		 * Removes the BoneWeights and BoneIds fields in the vertices. The BoneWeights 
		 * and BoneIds fields are added and removed in unison, and should always be the 
		 * same tuple size. <br>(Field names: "BoneWeights" & "BoneIds", Tuple size: varying 
		 * , Stored as: Vertex attribute) 
		 */
		virtual	void RemoveBoneWeights(  ) = 0;

		/**
		 * Gets the BoneWeights field. <br>(Field name: "BoneWeights", Tuple size: varying 
		 *  , Stored as: Vertex attribute) 
		 * @return the bone weights array 
		 */
		virtual	CountedPointer<IRealArray> GetBoneWeights(  ) = 0;

		/**
		 * Gets the BoneIds field. <br>(Field name: "BoneIds", Tuple size: varying , Stored 
		 *  as: Vertex attribute) 
		 * @return the bone id array 
		 */
		virtual	CountedPointer<IRidArray> GetBoneIds(  ) = 0;

		/**
		 * Adds the VertexWeighting field for the vertices. <br>(Field name: "VertexWeighting", 
		 *  Tuple size: 1 , Stored as: Vertex attribute) 
		 */
		virtual	void AddVertexWeighting(  ) = 0;

		/**
		 * Removes the VertexWeighting field for the vertices. <br>(Field name: "VertexWeighting", 
		 *  Tuple size: 1 , Stored as: Vertex attribute) 
		 */
		virtual	void RemoveVertexWeighting(  ) = 0;

		/**
		 * Gets the VertexWeighting field for the vertices. <br>(Field name: "VertexWeighting", 
		 *  Tuple size: 1 , Stored as: Vertex attribute) 
		 * @return the vertex weighting array 
		 */
		virtual	CountedPointer<IRealArray> GetVertexWeighting(  ) = 0;

		/**
		 * Adds a TexCoords field in the vertices. The valid id range of the level parameter 
		 *  is 0-255, which equals fields 'TexCoords0' - 'TexCoords255' <br>(Field name: 
		 * "TexCoords0" - "TexCoords255", Tuple size: 2 , Stored as: Vertex attribute) 
		 * @param level is the level of the new texcoord field 
		 */
		virtual	void AddTexCoords( rid level ) = 0;

		/**
		 * Removes a TexCoords field in the vertices. The valid id range of the level 
		 * parameter is 0-255, which equals fields 'TexCoords0' - 'TexCoords255' <br>(Field 
		 * name: "TexCoords0" - "TexCoords255", Tuple size: 2 , Stored as: Vertex attribute) 
		 * @param level is the level of the texcoord field to remove 
		 */
		virtual	void RemoveTexCoords( rid level ) = 0;

		/**
		 * Adds/Removes/Gets a TexCoords field in the vertices. The valid id range of 
		 * the level parameter is 0-255, which equals fields 'TexCoords0' - 'TexCoords255' 
		 * <br>(Field name: "TexCoords0" - "TexCoords255", Tuple size: 2 , Stored as: 
		 * Vertex attribute) 
		 * @param level is the level of the texcoord field to get 
		 * @return the texcoord array for the requested level 
		 */
		virtual	CountedPointer<IRealArray> GetTexCoords( rid level ) = 0;

		/**
		 * Adds a Tangents field in the vertices. <br>(Field name: "Tangents0" - "Tangents255" 
		 *  & "Bitangents0" - "Bitangents255", Tuple size: 3 , Stored as: Vertex attribute) 
		 * @param level is the level of the new tangent fields 
		 */
		virtual	void AddTangents( rid level ) = 0;

		/**
		 * Removes a Tangents field in the vertices. <br>(Field name: "Tangents0" - "Tangents255" 
		 *  & "Bitangents0" - "Bitangents255", Tuple size: 3 , Stored as: Vertex attribute) 
		 * @param level is the level of the tangent fields to remove 
		 */
		virtual	void RemoveTangents( rid level ) = 0;

		/**
		 * Gets a Tangents field in the vertices. <br>(Field name: "Tangents0" - "Tangents255" 
		 *  & "Bitangents0" - "Bitangents255", Tuple size: 3 , Stored as: Vertex attribute) 
		 * @param level is the level of the tangent field to get 
		 * @return the tangent array for the requested level 
		 */
		virtual	CountedPointer<IRealArray> GetTangents( rid level ) = 0;

		/**
		 * Gets a Bitangents field in the vertices. <br>(Field name: "Tangents0" - "Tangents255" 
		 *  & "Bitangents0" - "Bitangents255", Tuple size: 3 , Stored as: Vertex attribute) 
		 * @param level is the level of the bitangent field to get 
		 * @return the bitangent array for the requested level 
		 */
		virtual	CountedPointer<IRealArray> GetBitangents( rid level ) = 0;

		/**
		 * Adds the Normals field in the vertices. <br>(Field name: "Normals" , Tuple 
		 * size: 3 , Stored as: Vertex attribute) 
		 */
		virtual	void AddNormals(  ) = 0;

		/**
		 * Removes the Normals field in the vertices. <br>(Field name: "Normals" , Tuple 
		 * size: 3 , Stored as: Vertex attribute) 
		 */
		virtual	void RemoveNormals(  ) = 0;

		/**
		 * Gets the Normals field in the vertices. <br>(Field name: "Normals" , Tuple 
		 * size: 3 , Stored as: Vertex attribute) 
		 * @return the normals array 
		 */
		virtual	CountedPointer<IRealArray> GetNormals(  ) = 0;

		/**
		 * Adds a Color field in the vertices. The valid id range of the level parameter 
		 * is 0-255, which equals fields 'Colors0' - 'Colors255' <br>(Field name: "Colors0" 
		 * - "Colors255", Tuple size: 4 , Stored as: Vertex attribute) 
		 * @param level is the level of the Color field to add 
		 */
		virtual	void AddColors( rid level ) = 0;

		/**
		 * Removes a Color field in the vertices. The valid id range of the level parameter 
		 *  is 0-255, which equals fields 'Colors0' - 'Colors255' <br>(Field name: "Colors0" 
		 *  - "Colors255", Tuple size: 4 , Stored as: Vertex attribute) 
		 * @param level is the level of the Color field to remove 
		 */
		virtual	void RemoveColors( rid level ) = 0;

		/**
		 * Gets a Color field in the vertices. The valid id range of the level parameter 
		 * is 0-255, which equals fields 'Colors0' - 'Colors255' <br>(Field name: "Colors0" 
		 * - "Colors255", Tuple size: 4 , Stored as: Vertex attribute) 
		 * @param level is the level of the Colors field to get 
		 * @return the colors array 
		 */
		virtual	CountedPointer<IRealArray> GetColors( rid level ) = 0;

		/**
		 * Adds a user-specified vertex field from an already populated value array. To 
		 * be able to add a field, it must have a name that does not conflict with the 
		 * existing vertex fields. 
		 * @param field is the field that is to be added 
		 */
		virtual	void AddUserVertexField( IValueArray *field ) = 0;

		/**
		 * Removes a user-specified vertex field. 
		 * @param name is the name of the field that is to be removed 
		 */
		virtual	void RemoveUserVertexField( const char * name ) = 0;

		/**
		 * Gets a user-specified vertex field. 
		 * @param name is the name of the field to be fetched 
		 * @return the matching field, or NULL if no such field exists 
		 */
		virtual	CountedPointer<IValueArray> GetUserVertexField( const char * name ) = 0;

		/**
		 * Adds an empty user-specified vertex field. To be able to add a field, it must 
		 * have a name that does not conflict with the existing vertex fields. 
		 * @param base_type is the type of data in the field, the alternatives can be 
		 * found in SimplygonSDK::BaseTypes 
		 * @param name is the name of the new field 
		 * @param tuple_size is the tuple size of the new field 
		 * @return the newly added field 
		 */
		virtual	CountedPointer<IValueArray> AddBaseTypeUserVertexField( rid base_type , const char * name , unsigned int tuple_size ) = 0;

		/**
		 * Retrieves the IFieldData object that contains the vertex fields 
		 * @return the vertex fields data 
		 */
		virtual	CountedPointer<IFieldData> GetVertices(  ) = 0;

		/**
		 * Gets the VertexIds field in the triangles. 
		 * @return the vertex ids array 
		 */
		virtual	CountedPointer<IRidArray> GetVertexIds(  ) = 0;

		/**
		 * Adds the MaterialIds field in the triangles. <br>(Field name: "MaterialIds", 
		 * Tuple size: 1 , Stored as: triangle attribute) 
		 */
		virtual	void AddMaterialIds(  ) = 0;

		/**
		 * Removes the MaterialIds field in the triangles. <br>(Field name: "MaterialIds", 
		 *  Tuple size: 1 , Stored as: triangle attribute) 
		 */
		virtual	void RemoveMaterialIds(  ) = 0;

		/**
		 * Gets the MaterialIds field in the triangles. <br>(Field name: "MaterialIds", 
		 * Tuple size: 1 , Stored as: triangle attribute) 
		 * @return the material ids array 
		 */
		virtual	CountedPointer<IRidArray> GetMaterialIds(  ) = 0;

		/**
		 * Adds the GroupIds field in the triangles. <br>(Field name: "GroupIds", Tuple 
		 * size: 1 , Stored as: triangle attribute) 
		 */
		virtual	void AddGroupIds(  ) = 0;

		/**
		 * Removes the GroupIds field in the triangles. <br>(Field name: "GroupIds", Tuple 
		 *  size: 1 , Stored as: triangle attribute) 
		 */
		virtual	void RemoveGroupIds(  ) = 0;

		/**
		 * Gets the GroupIds field in the triangles. <br>(Field name: "GroupIds", Tuple 
		 * size: 1 , Stored as: triangle attribute) 
		 * @return the material ids array 
		 */
		virtual	CountedPointer<IRidArray> GetGroupIds(  ) = 0;

		/**
		 * Adds a user-specified triangle field from an already populated array. To be 
		 * able to add a field, it must have a name that does not conflict with the existing 
		 * triangle fields. 
		 * @param field is the array that is to be added 
		 */
		virtual	void AddUserTriangleField( IValueArray *field ) = 0;

		/**
		 * Removes a user-specified triangle field. To be able to add a field, it must 
		 * have a name that does not conflict with the existing triangle fields. 
		 * @param name name of the field to be removed 
		 */
		virtual	void RemoveUserTriangleField( const char * name ) = 0;

		/**
		 * Gets a user-specified triangle field. To be able to add a field, it must have 
		 * a name that does not conflict with the existing triangle fields. 
		 * @param name name of the field to be fetched 
		 */
		virtual	CountedPointer<IValueArray> GetUserTriangleField( const char * name ) = 0;

		/**
		 * Adds an empty user-specified triangle field. To be able to add a field, it 
		 * must have a name that does not conflict with the existing triangle fields. 
		 * @param base_type is the type of data in the field, the alternatives can be 
		 * found in SimplygonSDK::BaseTypes 
		 * @param name is the name of the new field 
		 * @param tuple_size is the tuple size of the new field 
		 * @return the newly added field 
		 */
		virtual	CountedPointer<IValueArray> AddBaseTypeUserTriangleField( rid base_type , const char * name , unsigned int tuple_size ) = 0;

		/**
		 * Retrieves the ReFieldData object that contains the triangle fields 
		 * @return the triangle fields data 
		 */
		virtual	CountedPointer<IFieldData> GetTriangles(  ) = 0;

		/**
		 * Get the inferior (minimum) extent of the geometry 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetInf( real *realReturnDataPtr ) = 0;

		/**
		 * Set the inferior (minimum) extent of the geometry 
		 * @param vec_realInputDataPtr is the values that the minimum extent of the geometry 
		 * is to be set to 
		 */
		virtual	void SetInf( const real *vec_realInputDataPtr ) = 0;

		/**
		 * Get the superior (maximum) extent of the geometry 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetSup( real *realReturnDataPtr ) = 0;

		/**
		 * Set the superior (maximum) extent of the geometry 
		 * @param vec_realInputDataPtr is the values that the maximum extent of the geometry 
		 * is to be set to 
		 */
		virtual	void SetSup( const real *vec_realInputDataPtr ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AddColors AddColors() \endlink instead. DiffuseColors is now an alias 
		 * for 'Colors0' 
		 */
		virtual	SGDEPRECATED void AddDiffuseColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RemoveColors RemoveColors() \endlink instead. DiffuseColors is now 
		 * an alias for 'Colors0' 
		 */
		virtual	SGDEPRECATED void RemoveDiffuseColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetColors GetColors() \endlink instead. DiffuseColors is now an alias 
		 * for 'Colors0' 
		 */
		virtual	SGDEPRECATED CountedPointer<IRealArray> GetDiffuseColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AddColors AddColors() \endlink instead. SpecularColors is now an 
		 * alias for 'Colors1' 
		 */
		virtual	SGDEPRECATED void AddSpecularColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RemoveColors RemoveColors() \endlink instead. SpecularColors is now 
		 * an alias for 'Colors1' 
		 */
		virtual	SGDEPRECATED void RemoveSpecularColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetColors GetColors() \endlink instead. SpecularColors is now an 
		 * alias for 'Colors1' 
		 */
		virtual	SGDEPRECATED CountedPointer<IRealArray> GetSpecularColors(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link AddTangents AddTangents() \endlink instead. Bitangents are Added 
		 * and Removed in unison with Tangents. Only need to call AddTangents to add both 
		 * Tangent and Bitangent fields, and RemoveTangents to remove both fields. Use 
		 * GetBitangents to retrieve the bitangent field. 
		 * @param level is the level of the new field 
		 */
		virtual	SGDEPRECATED void AddBitangents( rid level ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RemoveTangents RemoveTangents() \endlink instead. Bitangents are 
		 * Added and Removed in unison with Tangents. Only need to call AddTangents to 
		 * add both Tangent and Bitangent fields, and RemoveTangents to remove both fields. 
		 *  Use GetBitangents to retrieve the bitangent field. 
		 * @param level is the level of the field to be removed 
		 */
		virtual	SGDEPRECATED void RemoveBitangents( rid level ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IPackedGeometryData
	 */
	typedef CountedPointer<IPackedGeometryData> spPackedGeometryData;

	/// Conditional build define for the interface IPackedGeometryData
	#define InterfaceDefined_SimplygonSDK_IPackedGeometryData 1

	class IValueArray;
	class IFieldData;

	/**
	 * IImageData holds unpacked image data. The data is stored in an IFieldData object 
	 *  as value fields. For simplicity all images are implemented as 3D images, but 
	 * has an extent of 1 in Z for 2D images, and an extent of 1 in both Y and Z for 
	 * 1D images. Cube maps are stored as a 3D image with Z-depth of 6 (one for each 
	 * side) 
	 */
	class IImageData : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IImageData is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IImageData",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IImageData pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IImageData pointer
		 * @return a pointer to the IImageData object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IImageData *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IImageData") )
				return static_cast<IImageData*>(ptr);
			return NULL;
			}

		/**
		 * Creates another image data object with the same field setup. To also copy the 
		 * data to the new object, set copy_data to true. 
		 * @param copy_data if true, all data is copied 
		 * @return the new copy of the image data object 
		 */
		virtual	CountedPointer<IImageData> NewCopy( bool copy_data ) = 0;

		/**
		 * Copies the field setup and data from another object. To only copy the setup, 
		 * set copy_data to false. 
		 * @param source the IImageData object to copy from 
		 * @param copy_data true if the data should be copied along with the image data 
		 * properties 
		 */
		virtual	void DeepCopy( IImageData *source , bool copy_data ) = 0;

		/**
		 * Adds the Colors field. 
		 * @param valuetype is the type used to store the tuples. Use TYPES_ID_UCHAR base 
		 * type for 8-bit per channel data, and TYPES_ID_USHORT base type for 16-bit per 
		 * channel data. 
		 * @param format is a string that describes the format of the color field. Each 
		 * character in format represents a color channel in the color tuple. All characters 
		 * in the format string must be unique and uppercase. Normal values for format 
		 * are: RGB, RGBA, YUV, HLS, CMYK, L and R. In these examples the tuplesize of 
		 * the formats will be 3,4,3,3,4,1 and 1, respectively. The format string must 
		 * be at least one character long. Use the SG_IMAGEDATA_FORMAT_[] defines to define 
		 * the standard types. 
		 */
		virtual	void AddColors( rid valuetype , const char * format ) = 0;

		/**
		 * Removes the Colors field. 
		 */
		virtual	void RemoveColors(  ) = 0;

		/**
		 * Gets the Colors field. 
		 * @return the value array used by the colors field. 
		 */
		virtual	CountedPointer<IValueArray> GetColors(  ) = 0;

		/**
		 * Gets the base type used by the colors field 
		 * @return the base type used by the colors field. 
		 */
		virtual	rid GetColorsValueType(  ) = 0;

		/**
		 * Checks if the image has a color field 
		 * @return true if the image has a color field 
		 */
		virtual	bool HasColors(  ) = 0;

		/**
		 * Gets the color field format specification 
		 * @return the format specification string 
		 */
		virtual	rstring GetColorsFormat(  ) = 0;

		/**
		 * Adds a field. The field must have a name that does not conflict with any of 
		 * the existing fields' names. The field is referenced by the image after the 
		 * call to AddField() 
		 * @param name the name of the field to add 
		 * @param valuetype the type of the field, types listed in SimplygonSDK::BaseTypes 
		 *  
		 * @param tuplesize the size of tuples in the new field 
		 */
		virtual	void AddField( const char * name , rid valuetype , unsigned int tuplesize ) = 0;

		/**
		 * Removes a field. 
		 * @param name the name of the field to remove 
		 */
		virtual	void RemoveField( const char * name ) = 0;

		/**
		 * Gets a field. 
		 * @param name the name of the field to get 
		 * @return the requested field 
		 */
		virtual	CountedPointer<IValueArray> GetField( const char * name ) = 0;

		/**
		 * Gets a field's value type. 
		 * @param name the name of the field whose type to get, types listed in SimplygonSDK::BaseTypes 
		 *  
		 * @return the base type of the field 
		 */
		virtual	rid GetFieldValueType( const char * name ) = 0;

		/**
		 * Check if a field exists in the IImageData object. 
		 * @param name the name of the field to check for 
		 * @return true if field exists, otherwise false 
		 */
		virtual	bool HasField( const char * name ) = 0;

		/**
		 * Get the type of this image data. 
		 * @return the type of this image data 
		 */
		virtual	unsigned int GetType(  ) = 0;

		/**
		 * Get the X dimension size of this image data. 
		 * @return the X dimension size of this image data. 
		 */
		virtual	unsigned int GetXSize(  ) = 0;

		/**
		 * Get the Y dimension size of this image data. 
		 * @return the Y dimension size of this image data. 
		 */
		virtual	unsigned int GetYSize(  ) = 0;

		/**
		 * Get the Z dimension size of this image data. 
		 * @return the Z dimension size of this image data. 
		 */
		virtual	unsigned int GetZSize(  ) = 0;

		/**
		 * Set the X dimension size of this image data. 
		 * @param _xs the desired X dimension size of this image data 
		 */
		virtual	void Set1DSize( unsigned int _xs ) = 0;

		/**
		 * Set the X and Y dimension size of this image data. 
		 * @param _xs the desired X dimension size of this image data 
		 * @param _ys the desired Y dimension size of this image data 
		 */
		virtual	void Set2DSize( unsigned int _xs , unsigned int _ys ) = 0;

		/**
		 * Set the X, Y and Z dimension size of this image data. 
		 * @param _xs the desired X dimension size of this image data 
		 * @param _ys the desired Y dimension size of this image data 
		 * @param _zs the desired Z dimension size of this image data 
		 */
		virtual	void Set3DSize( unsigned int _xs , unsigned int _ys , unsigned int _zs ) = 0;

		/**
		 * Set the 6 uniform cube side dimensions of this image data. 
		 * @param _xs the desired X dimension size of each cube side in this image data 
		 *  
		 * @param _ys the desired Y dimension size of each cube side in this image data 
		 *  
		 */
		virtual	void SetCubeSize( unsigned int _xs , unsigned int _ys ) = 0;

		/**
		 * Clears all data from the image, and removes all data fields. The type is reset 
		 * to 2D 
		 */
		virtual	void Reset(  ) = 0;

		/**
		 * Get the ImageFields field data object, that holds all fields within the image 
		 * @return the ImageFields field data object 
		 */
		virtual	CountedPointer<IFieldData> GetImageFields(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IImageData
	 */
	typedef CountedPointer<IImageData> spImageData;

	/// Conditional build define for the interface IImageData
	#define InterfaceDefined_SimplygonSDK_IImageData 1

	class IObject;

	/**
	 * ITable is the base class for tables objects, where items are added into an id-indexed 
	 *  array of objects. 
	 */
	class ITable : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ITable is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ITable",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ITable pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ITable pointer
		 * @return a pointer to the ITable object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ITable *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ITable") )
				return static_cast<ITable*>(ptr);
			return NULL;
			}

		/**
		 * Adds an item to the table. The returned value is the id of the item within 
		 * the table, which can be used to retrieve the item using GetItem(). 
		 * @param item is the item that is to be added to the table 
		 * @return the id of the added item in the table 
		 */
		virtual	rid AddItem( IObject *item ) = 0;

		/**
		 * Sets an item in the table. The id must exist in the table. 
		 * @param id is the id that is to be set 
		 * @param item is the object set to the id 
		 */
		virtual	void SetItem( rid id , IObject *item ) = 0;

		/**
		 * Finds a named item in the table. If multiple items have the same name, the 
		 * first will be returned. If no item was found, the return will be NULL. 
		 * @param name is the string that is to be searched for 
		 * @return the matching object, or NULL if no match was found 
		 */
		virtual	CountedPointer<IObject> FindItem( const char * name ) = 0;

		/**
		 * Finds the id of an item in the table, using the name. If multiple items have 
		 * the same name, the first will be returned. If no item was found, the return 
		 * will be -1. 
		 * @param name is the string that is to be searched for 
		 * @return the id of the found object, or -1 if no match was found 
		 */
		virtual	rid FindItemId( const char * name ) = 0;

		/**
		 * Returns the i:th item, using the id of the item 
		 * @param id is the id of the requested object 
		 * @return the found object 
		 */
		virtual	CountedPointer<IObject> GetItem( rid id ) = 0;

		/**
		 * Removes an item, and invalidates the id. The id will not point at a valid item, 
		 *  until Clear is called and new items are added up until the id. 
		 * @param id the id of the object that should be removed 
		 */
		virtual	void RemoveItem( rid id ) = 0;

		/**
		 * Clears the table, and removes all items. If AddItem is called after Clear, 
		 * the item ids will start over from 0 
		 */
		virtual	void Clear(  ) = 0;

		/**
		 * Gets the number of item id:s in the table. If an item has been removed, using 
		 * RemoveItem, the id is still counted in this method. GetIdsCount also equals 
		 * the id that will be applied to the next item added to the table. 
		 * @return the itemcount of the table 
		 */
		virtual	unsigned int GetItemsCount(  ) = 0;

		/**
		 * Clears the current table and copies the items from a source table. 
		 * @param src the table to copy 
		 */
		virtual	void Copy( ITable *src ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ITable
	 */
	typedef CountedPointer<ITable> spTable;

	/// Conditional build define for the interface ITable
	#define InterfaceDefined_SimplygonSDK_ITable 1

	class IShadingNode;

	/**
	 * IMaterial keeps information on materials used while rendering. 
	 */
	class IMaterial : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IMaterial is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IMaterial",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IMaterial pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IMaterial pointer
		 * @return a pointer to the IMaterial object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IMaterial *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IMaterial") )
				return static_cast<IMaterial*>(ptr);
			return NULL;
			}

		/**
		 * Returns the number of material channels in the material 
		 * @return the current channel count 
		 */
		virtual	unsigned int GetChannelCount(  ) = 0;

		/**
		 * Returns the name of a channel from the index. Note that by adding/removing 
		 * channels the indices are changed, and this method should only be used for enumeration 
		 * @param index is the index to the channel to be fetched 
		 * @return the channel name corresponding to the index 
		 */
		virtual	rstring GetChannelFromIndex( rid index ) = 0;

		/**
		 * Add a user material channel 
		 * @param channel_name the name of the material channel 
		 */
		virtual	void AddUserChannel( const char * channel_name ) = 0;

		/**
		 * Remove a user material channel 
		 * @param channel_name the name of the material channel 
		 */
		virtual	void RemoveUserChannel( const char * channel_name ) = 0;

		/**
		 * Check if the user channel exists within the material. 
		 * @param channel_name the name of the material channel 
		 * @return true if the channel exists, false if not 
		 */
		virtual	bool HasUserChannel( const char * channel_name ) = 0;

		/**
		 * Set which shading network to use for the material's channel 
		 * @param channel_name is the name of the new channel 
		 * @param node the shading network's exit node 
		 */
		virtual	void SetShadingNetwork( const char * channel_name , IShadingNode *node ) = 0;

		/**
		 * Get which shading network to use for the material's channel 
		 * @param channel_name is the name of the channel 
		 * @return the current shading network's exit node 
		 */
		virtual	CountedPointer<IShadingNode> GetShadingNetwork( const char * channel_name ) = 0;

		/**
		 * Save shading network to XML 
		 * @param channel_name is the name of the channel 
		 * @param in_xml the xml descriping the shading node graph 
		 * @return the current shading network's exit node 
		 */
		virtual	void LoadShadingNetworkFromXML( const char * channel_name , const char * in_xml ) = 0;

		/**
		 * Save shading network to XML 
		 * @param channel_name is the name of the channel 
		 * @return the current shading network's exit node 
		 */
		virtual	rstring SaveShadingNetworkToXML( const char * channel_name ) = 0;

		/**
		 * Set/Get the UseTangentSpaceNormals flag. If set, the normal map has tangent 
		 * space normals 
		 * @param value the desired flag value. 
		 */
		virtual	void SetUseTangentSpaceNormals( bool value ) = 0;

		/**
		 * Set/Get the UseTangentSpaceNormals flag. If set, the normal map has tangent 
		 * space normals 
		 */
		virtual	bool GetUseTangentSpaceNormals(  ) = 0;

		/**
		 * Returns true if the material is setup identical to this material 
		 * @param mat the material to compare to 
		 */
		virtual	bool IsIdenticalTo( IMaterial *mat ) = 0;

		/**
		 * Returns true if the material could be transparent 
		 */
		virtual	bool IsTransparent(  ) = 0;

		/**
		 * Gets the unique id for this node. 
		 * @return the GUID for this node 
		 */
		virtual	rstring GetMaterialGUID(  ) = 0;

		/**
		 * Sets the unique id for this node. 
		 * @param value is the node id of type string 
		 */
		virtual	void SetMaterialGUID( const char * value ) = 0;

		/**
		 * Set the MaterialType. 
		 * @param value the Material type. One of the following<br><ul><li>SG_MATERIALTYPE_STANDARD 
		 * -> Use the standard shader</li><li>SG_MATERIALTYPE_FLIPBOOK -> Use the flipbook 
		 * shader.</li><li>SG_MATERIALTYPE_BILLBOARDCLOUD -> Use the billboard cloud shader. 
		 * </li></ul> 
		 */
		virtual	void SetMaterialType( unsigned int value ) = 0;

		/**
		 * Get the MaterialType. 
		 */
		virtual	unsigned int GetMaterialType(  ) = 0;

		/**
		 * Changes the prefix path (drive, directory) of all matching texture paths in 
		 * the material, with the new one. This method is useful when external textures 
		 * are placed in a specific folder, which needs to be moved 
		 * @param current_path_prefix the current prefix path, either relative or absolute 
		 *  
		 * @param new_path_prefix the new prefix path 
		 */
		virtual	void ChangeTexturePrefixPath( const char * current_path_prefix , const char * new_path_prefix ) = 0;

		/**
		 * Maps the alpha component of the diffuse channel to all the RGBA components 
		 * in the opacity channel (by using a swizzle node). 
		 */
		virtual	void SetOpacityFromDiffuseAlpha(  ) = 0;

		/**
		 * Converts parameters such as UVTiling and UVOffset in shading networks. 
		 */
		virtual	void ConvertHandedness(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IMaterial
	 */
	typedef CountedPointer<IMaterial> spMaterial;

	/// Conditional build define for the interface IMaterial
	#define InterfaceDefined_SimplygonSDK_IMaterial 1

	class IMaterial;
	class IRidArray;

	/**
	 * IMaterialTable keeps information on materials used while rendering. 
	 */
	class IMaterialTable : public ITable
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IMaterialTable is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IMaterialTable",type)==0 )
				return true;
			return ITable::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IMaterialTable pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IMaterialTable pointer
		 * @return a pointer to the IMaterialTable object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IMaterialTable *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IMaterialTable") )
				return static_cast<IMaterialTable*>(ptr);
			return NULL;
			}

		/**
		 * Adds a material to the table. The returned value is the id of the material 
		 * within the table. 
		 * @param material the material to add into the table 
		 * @return the material id of the material in the table 
		 */
		virtual	rid AddMaterial( IMaterial *material ) = 0;

		/**
		 * Finds a material in the table, using the material name. If multiple materials 
		 * have the same name, the first will be returned. If the material was not found, 
		 * the return will be NULL. 
		 * @param name the material name to look for 
		 * @return the material, if found, NULL otherwise 
		 */
		virtual	CountedPointer<IMaterial> FindMaterial( const char * name ) = 0;

		/**
		 * Finds a material in the table that is setup identical to the specified material. 
		 * @param mat the material setup to look for 
		 * @return the material, if found, NULL otherwise 
		 */
		virtual	CountedPointer<IMaterial> FindIdenticalMaterial( IMaterial *mat ) = 0;

		/**
		 * Finds the id of a material in the table, using the material name. If multiple 
		 * materials have the same name, the first will be returned. If the material was 
		 * not found, the return will be -1. 
		 * @param name the material name to look for 
		 * @return the material id if found, -1 otherwise 
		 */
		virtual	rid FindMaterialId( const char * name ) = 0;

		/**
		 * Finds the id of a material in the table that is setup identical to the specified 
		 *  material. 
		 * @param mat the material setup to look for 
		 * @return the material id if found, -1 otherwise 
		 */
		virtual	rid FindIdenticalMaterialId( IMaterial *mat ) = 0;

		/**
		 * Finds the materials in the table that are setup identical to the specified 
		 * material. 
		 * @param mat the material setup to look for 
		 * @return the an array material ids if found, NULL otherwise 
		 */
		virtual	CountedPointer<IRidArray> FindIdenticalMaterialIds( IMaterial *mat ) = 0;

		/**
		 * Sets a material in the table. 
		 * @param id the id of the material in the table 
		 * @param material the material to set into the table 
		 */
		virtual	void SetMaterial( rid id , IMaterial *material ) = 0;

		/**
		 * Returns the i:th material, using the id of the material 
		 * @param id the id of the material in the table 
		 * @return the the material in the table 
		 */
		virtual	CountedPointer<IMaterial> GetMaterial( rid id ) = 0;

		/**
		 * Removes a material. If GetMaterial is called with the material id, the value 
		 * returned will be NULL. 
		 * @param id the id of the material in the table 
		 */
		virtual	void RemoveMaterial( rid id ) = 0;

		/**
		 * Returns the number of materials in the table 
		 */
		virtual	unsigned int GetMaterialsCount(  ) = 0;

		/**
		 * Changes the prefix path (drive, directory) of all matching texture paths in 
		 * all the materials in the table, with the new one. This method is useful when 
		 * external textures are placed in a specific folder, which needs to be moved 
		 * @param current_path_prefix the current prefix path, either relative or absolute 
		 *  
		 * @param new_path_prefix the new prefix path 
		 */
		virtual	void ChangeTexturePrefixPath( const char * current_path_prefix , const char * new_path_prefix ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IMaterialTable
	 */
	typedef CountedPointer<IMaterialTable> spMaterialTable;

	/// Conditional build define for the interface IMaterialTable
	#define InterfaceDefined_SimplygonSDK_IMaterialTable 1

	class IRealArray;

	/**
	 * IMatrix4x4 is used to represent and manipulate 4x4 transformation matrices, 
	 * which can be either standard matrices, or homogeneous 4x4 matrices used to transform 
	 * 3D homogeneous coordinates [x y z w]. The transformations are defined in row-major 
	 *  order. 
	 */
	class IMatrix4x4 : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IMatrix4x4 is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IMatrix4x4",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IMatrix4x4 pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IMatrix4x4 pointer
		 * @return a pointer to the IMatrix4x4 object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IMatrix4x4 *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IMatrix4x4") )
				return static_cast<IMatrix4x4*>(ptr);
			return NULL;
			}

		/**
		 * Copies the contents of an input matrix to the current matrix 
		 * @param mtxin is the source matrix 
		 */
		virtual	void DeepCopy( IMatrix4x4 *mtxin ) = 0;

		/**
		 * Set the current matrix to the zero matrix (all zeros). 
		 */
		virtual	void Zero(  ) = 0;

		/**
		 * Set the current matrix to the identity matrix. 
		 */
		virtual	void Identity(  ) = 0;

		/**
		 * Get a matrix element. 
		 * @param column is column index 
		 * @param row is the row index 
		 * @return the value of the element 
		 */
		virtual	real GetElement( unsigned int column , unsigned int row ) = 0;

		/**
		 * Set a matrix element. 
		 * @param column is column index 
		 * @param row is the row index 
		 * @param value is the value to which the element will be set 
		 */
		virtual	void SetElement( unsigned int column , unsigned int row , real value ) = 0;

		/**
		 * Gets the elements of the matrix 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 16 elements in size 
		 * @return an array containing all elements 
		 */
		virtual	void GetElements( real *realReturnDataPtr ) = 0;

		/**
		 * Transpose the input matrix and save it to the current matrix. 
		 * @param mtxin is the input matrix 
		 */
		virtual	void Transpose( IMatrix4x4 *mtxin ) = 0;

		/**
		 * Transpose the current matrix 
		 */
		virtual	void Transpose(  ) = 0;

		/**
		 * Invert the input matrix and save it to the current matrix. 
		 * @param mtxin is the input matrix 
		 */
		virtual	void Invert( IMatrix4x4 *mtxin ) = 0;

		/**
		 * Invert the current matrix. 
		 */
		virtual	void Invert(  ) = 0;

		/**
		 * Returns the top 3x3 determinant 
		 */
		virtual	real Determinant3x3(  ) = 0;

		/**
		 * Multiply in-place an array with 3D points [x y z 1]. 
		 * @param vecarray is the vectors to be multiplied and written to 
		 */
		virtual	void Point3ArrayMultiply( IRealArray *vecarray ) = 0;

		/**
		 * Multiply in-place an array with 3D direction vectors [x y z 0]. 
		 * @param vecarray is the vectors to be multiplied and written to 
		 */
		virtual	void Direction3ArrayMultiply( IRealArray *vecarray ) = 0;

		/**
		 * Multiply in-place an array with full 4D vectors [x y z w]. 
		 * @param vecarray is the vectors to be multiplied and written to 
		 */
		virtual	void Vector4ArrayMultiply( IRealArray *vecarray ) = 0;

		/**
		 * Creates a 3D rotation in a homogeneous transformation 4x4 matrix around the 
		 * specified axis. 
		 * @param angle is the angle 
		 * @param ax is the x component of the rotation axis 
		 * @param ay is the y component of the rotation axis 
		 * @param az is the z component of the rotation axis 
		 */
		virtual	void SetToRotationTransform( real angle , real ax , real ay , real az ) = 0;

		/**
		 * Creates a 3D scaling in a homogeneous transformation 4x4 matrix 
		 * @param sx is the x component of the scaling 
		 * @param sy is the y component of the scaling 
		 * @param sz is the z component of the scaling 
		 */
		virtual	void SetToScalingTransform( real sx , real sy , real sz ) = 0;

		/**
		 * Creates a 3D translation in a homogeneous 4x4 matrix. 
		 * @param tx is the x component of the scaling 
		 * @param ty is the y component of the scaling 
		 * @param tz is the z component of the scaling 
		 */
		virtual	void SetToTranslationTransform( real tx , real ty , real tz ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IMatrix4x4
	 */
	typedef CountedPointer<IMatrix4x4> spMatrix4x4;

	/// Conditional build define for the interface IMatrix4x4
	#define InterfaceDefined_SimplygonSDK_IMatrix4x4 1

	class IMatrix4x4;
	class IRealArray;

	/**
	 * ITransform3 handles homogeneous 4x4 transforms, i.e. transformations which can 
	 * be represented by multiplying a 4x4 matrix with a homogeneous 3D coordinate. 
	 * <br> ITransform3 can either pre-multiply a matrix onto the transformation, which 
	 * will add a transform around the current transform, or it can post-multiply the 
	 * matrix, which will add a transform inside the current transform. Post-multiply 
	 * is the default mode. 
	 */
	class ITransform3 : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ITransform3 is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ITransform3",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ITransform3 pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ITransform3 pointer
		 * @return a pointer to the ITransform3 object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ITransform3 *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ITransform3") )
				return static_cast<ITransform3*>(ptr);
			return NULL;
			}

		/**
		 * Resets the transformation to the identity transform, resets the multiplication 
		 * mode to postmultiply, and clears the matrix stack 
		 */
		virtual	void Clear(  ) = 0;

		/**
		 * Get the matrix that is used to specify the transform 
		 * @return the transform matrix 
		 */
		virtual	CountedPointer<IMatrix4x4> GetMatrix(  ) = 0;

		/**
		 * Get the real array that is used to store the matrix stack 
		 * @return the matrix stack array 
		 */
		virtual	CountedPointer<IRealArray> GetStack(  ) = 0;

		/**
		 * Set the transform concatenation mode to post-multiply, any added transform 
		 * will be concatenated using this mode. 
		 */
		virtual	void PostMultiply(  ) = 0;

		/**
		 * Set the transform concatenation mode to pre-multiply, any added transform will 
		 * be concatenated using this mode. 
		 */
		virtual	void PreMultiply(  ) = 0;

		/**
		 * Get the transform concatenation mode. 
		 * @return true if the mode is set to post-multiply, false otherwise 
		 */
		virtual	bool IsPostMultiply(  ) = 0;

		/**
		 * Get the transform concatenation mode. 
		 * @return true if the mode is set to pre-multiply, false otherwise 
		 */
		virtual	bool IsPreMultiply(  ) = 0;

		/**
		 * Concatenates a rotation transform to the current transform 
		 * @param angle the rotation angle around the axis 
		 * @param rx the x axis rotation component 
		 * @param ry the y axis rotation component 
		 * @param rz the z axis rotation component 
		 */
		virtual	void AddRotation( real angle , real rx , real ry , real rz ) = 0;

		/**
		 * Concatenates a scaling transform to the current transform 
		 * @param sx the scaling vector's x axis component 
		 * @param sy the scaling vector's y axis component 
		 * @param sz the scaling vector's z axis component 
		 */
		virtual	void AddScaling( real sx , real sy , real sz ) = 0;

		/**
		 * Concatenates a translation transform to the current transform 
		 * @param tx the translation vector's x component 
		 * @param ty the translation vector's y component 
		 * @param tz the translation vector's z component 
		 */
		virtual	void AddTranslation( real tx , real ty , real tz ) = 0;

		/**
		 * Concatenates a generic transform to the current transform 
		 * @param transform the input transform 
		 */
		virtual	void AddTransformation( IMatrix4x4 *transform ) = 0;

		/**
		 * Concatenates a generic transform to the current transform 
		 * @param transform the input transform 
		 */
		virtual	void AddTransformation( ITransform3 *transform ) = 0;

		/**
		 * Pushes the current transform onto the matrix stack 
		 */
		virtual	void Push(  ) = 0;

		/**
		 * Pops the topmost matrix on the the matrix stack, and replaces the current transform 
		 *  with the popped matrix. 
		 */
		virtual	void Pop(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ITransform3
	 */
	typedef CountedPointer<ITransform3> spTransform3;

	/// Conditional build define for the interface ITransform3
	#define InterfaceDefined_SimplygonSDK_ITransform3 1

	class IChunkedImageData;
	class IMappingImageMeshData;

	/**
	 * IMappingImage stores the mapping between a processed geometry and the source 
	 * geometry it is based on. The mapping image object also stores multisampling 
	 * settings. It can be used to cast information from the original geometry to the 
	 * processed geometry, such as surface transfer or calculating normal maps. See 
	 * IMaterialCaster and subclasses for casting uses. 
	 */
	class IMappingImage : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IMappingImage is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IMappingImage",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IMappingImage pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IMappingImage pointer
		 * @return a pointer to the IMappingImage object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IMappingImage *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IMappingImage") )
				return static_cast<IMappingImage*>(ptr);
			return NULL;
			}

		/**
		 * Return a copy of the mapping image which is scaled in size by half, in width 
		 * and height. The area covered is a quarter size of the original, hence it's 
		 * name. 
		 * @return copy of the mapping image, reduced in size to half of the original 
		 * width and height 
		 */
		virtual	CountedPointer<IMappingImage> NewQuartersizeCopy(  ) = 0;

		/**
		 * Get the width of the mapping texture. 
		 * @return the Width of the mapping texture 
		 */
		virtual	unsigned int GetWidth(  ) = 0;

		/**
		 * Get the height of the mapping texture. 
		 * @return the Height of the mapping texture 
		 */
		virtual	unsigned int GetHeight(  ) = 0;

		/**
		 * Get the number of layers of the mapping texture. 
		 * @return the LayerCount of the mapping texture 
		 */
		virtual	unsigned int GetLayerCount(  ) = 0;

		/**
		 * Get the multi sampling width of the mapping texture. 
		 * @return the multi sampling width 
		 */
		virtual	unsigned int GetMultisamplingWidth(  ) = 0;

		/**
		 * Get the multi sampling height of the mapping texture. 
		 * @return the multi sampling height 
		 */
		virtual	unsigned int GetMultisamplingHeight(  ) = 0;

		/**
		 * Get the ImageData object, which stores the mapping data 
		 * @return the ImageData object 
		 */
		virtual	CountedPointer<IChunkedImageData> GetImageData(  ) = 0;

		/**
		 * Get the IMappingImageMeshData Mesh data that is used to retrieve the triangle 
		 * mapping to mesh information to map back to the original scene. 
		 * @return the mapping mesh data object 
		 */
		virtual	CountedPointer<IMappingImageMeshData> GetMappingMeshData(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IMappingImage
	 */
	typedef CountedPointer<IMappingImage> spMappingImage;

	/// Conditional build define for the interface IMappingImage
	#define InterfaceDefined_SimplygonSDK_IMappingImage 1


	/**
	 * IProcessingObject is the abstract base class for all processing objects in Simplygon. 
	 *  Processing objects take parameters, exports the RunProcessing to run the main 
	 * processing and Clear to clear the internal states of the processing object. 
	 * All processing objects also export information on how far the process has moved 
	 * and the possibility to cancel long running processings. 
	 */
	class IProcessingObject : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IProcessingObject is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IProcessingObject",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IProcessingObject pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IProcessingObject pointer
		 * @return a pointer to the IProcessingObject object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IProcessingObject *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IProcessingObject") )
				return static_cast<IProcessingObject*>(ptr);
			return NULL;
			}

		/**
		 * Runs the processing the object is designed for 
		 */
		virtual	void RunProcessing(  ) = 0;

		/**
		 * Clear all internal states of the object. This will NOT clear the parameters 
		 * set in the object. 
		 */
		virtual	void Clear(  ) = 0;

		/**
		 * Returns the current progress of the processing as a value between 0 and 100 
		 * @return The current progress of the process as a value between 0 and 100. 
		 */
		virtual	real GetPercentDone(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IProcessingObject
	 */
	typedef CountedPointer<IProcessingObject> spProcessingObject;

	/// Conditional build define for the interface IProcessingObject
	#define InterfaceDefined_SimplygonSDK_IProcessingObject 1

	class IGeometryData;
	class IRealArray;

	/**
	 * Computes texture coordinates for arbitrary geometry. 
	 */
	class IParameterizer : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IParameterizer is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IParameterizer",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IParameterizer pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IParameterizer pointer
		 * @return a pointer to the IParameterizer object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IParameterizer *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IParameterizer") )
				return static_cast<IParameterizer*>(ptr);
			return NULL;
			}

		/**
		 * Executes the parameterization of the geometry. 
		 * @param geom is the geometry which is to be parameterized 
		 * @param arr will contain the completed parameterization 
		 * @return true if the parameterization succeeds, false otherwise 
		 */
		virtual	bool Parameterize( IGeometryData *geom , IRealArray *arr ) = 0;

		/**
		 * Set the maximum allowed texture stretch. Range 0->1 
		 * @param value is the value to which MaxStretch will be set 
		 */
		virtual	void SetMaxStretch( real value ) = 0;

		/**
		 * Get the maximum allowed texture stretch. Range 0->1 
		 * @return the current value of MaxStretch 
		 */
		virtual	real GetMaxStretch(  ) = 0;

		/**
		 * Set the number of input materials used in the original geometry for mapping 
		 * to multiple output materials. This needs to be set before you can set any specific 
		 * in-out material mapping. 0 signifies that no in-out material mapping is used, 
		 * ie. the process will produce one resulting mapping image. 
		 * @param value is the number of outputs 
		 */
		virtual	void SetInputMaterialCount( unsigned int value ) = 0;

		/**
		 * Get the number of input materials set earlier. 0 signifies that no in-out material 
		 *  mapping is used. 
		 * @return the current value of InputMaterialCount 
		 */
		virtual	unsigned int GetInputMaterialCount(  ) = 0;

		/**
		 * Set the number of output mapping images that are to be generated. The triangle 
		 * field OutputMaterialIds also needs to be set, or the material ids of the original 
		 * geometry needs to be set to a specific output material in this setting object. 
		 * @param value is the number of outputs Value cannot be less than 1. Value cannot 
		 * be greater than SG_MAXIMUM_PARAMETERIZER_OUTPUT_MATERIAL_COUNT. 
		 */
		virtual	void SetOutputMaterialCount( unsigned int value ) = 0;

		/**
		 * Get the number of output mapping images that are to be generated. 
		 * @return the current value of OutputMaterialCount 
		 */
		virtual	unsigned int GetOutputMaterialCount(  ) = 0;

		/**
		 * Set the material mapping for InMaterialId, meaning what material of the generated 
		 * LOD InMaterialId will be baked into. Both InputMaterialCount and OutputMaterialCount 
		 * need to be set for this mapping to work, and all original materials need to 
		 * be mapped to an existing output id. 
		 * @param InMaterialId is input material id 
		 * @param OutMaterialId is the output material id 
		 */
		virtual	void SetInputOutputMaterialMapping( unsigned int InMaterialId , int OutMaterialId ) = 0;

		/**
		 * Get the previously set material mapping for mat_id. 
		 * @param InMaterialId is input material id 
		 * @return the currently mapped output material for InMaterialId. -1 means mapping 
		 * is not set. 
		 */
		virtual	int GetInputOutputMaterialMapping( unsigned int InMaterialId ) = 0;

		/**
		 * Set the width of the texture to use. 
		 * @param value the texture width 
		 */
		virtual	void SetTextureWidth( unsigned int value ) = 0;

		/**
		 * Set the width of the texture to use. 
		 * @param id is the output material index 
		 * @param value the new output texture width 
		 */
		virtual	void SetTextureWidth( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the width of the texture to use. 
		 * @return the current texture width 
		 */
		virtual	unsigned int GetTextureWidth(  ) = 0;

		/**
		 * Get the width of the texture to use. 
		 * @param id the material id of the output material 
		 * @return the current texture width 
		 */
		virtual	unsigned int GetTextureWidth( unsigned int id ) = 0;

		/**
		 * Set the height of the texture to use. 
		 * @param value the new output texture height 
		 */
		virtual	void SetTextureHeight( unsigned int value ) = 0;

		/**
		 * Set the height of the texture to use. 
		 * @param id the output material id 
		 * @param value the new output texture height 
		 */
		virtual	void SetTextureHeight( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the height of the texture to use. 
		 * @return the current texture height 
		 */
		virtual	unsigned int GetTextureHeight(  ) = 0;

		/**
		 * Get the height of the texture to use. 
		 * @param id the output material id 
		 * @return the current texture height 
		 */
		virtual	unsigned int GetTextureHeight( unsigned int id ) = 0;

		/**
		 * Set the minimum number of pixels between charts. 
		 * @param value is the value to which GutterSpace will be set 
		 */
		virtual	void SetGutterSpace( unsigned int value ) = 0;

		/**
		 * Set the minimum number of pixels between charts. 
		 * @param id the output material id 
		 * @param value is the value to which GutterSpace will be set 
		 */
		virtual	void SetGutterSpace( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the minimum number of pixels between charts. 
		 * @return the current value of GutterSpace 
		 */
		virtual	unsigned int GetGutterSpace(  ) = 0;

		/**
		 * Get the minimum number of pixels between charts. 
		 * @param id the output material id 
		 * @return the current value of GutterSpace 
		 */
		virtual	unsigned int GetGutterSpace( unsigned int id ) = 0;

		/**
		 * Set the FeatureFlagsMask. Any edge that has one of the flags present as a feature 
		 *  edge, will have the edge forced as a parameterization feature. 
		 * @param value is the mask to which FeatureFlagsMask will be set 
		 */
		virtual	void SetFeatureFlagsMask( unsigned int value ) = 0;

		/**
		 * Get the FeatureFlagsMask. Any edge that has one of the flags present as a feature 
		 *  edge, will have the edge forced as a parameterization feature. 
		 * @return the current FeatureFlagsMask 
		 */
		virtual	unsigned int GetFeatureFlagsMask(  ) = 0;

		/**
		 * Set/Get the packing performance setting the value is in the range 0->1, where 
		 * 0 has the lowest packing efficiency, but runs the fastest, and 1 has the highest 
		 * packing efficiency but runs slower. 
		 * @param value is the value to which PackingEfficency will be set 
		 */
		virtual	void SetPackingEfficency( real value ) = 0;

		/**
		 * Set/Get the packing performance setting the value is in the range 0->1, where 
		 * 0 has the lowest packing efficiency, but runs the fastest, and 1 has the highest 
		 * packing efficiency but runs slower. 
		 * @return the current value of PackingEfficency 
		 */
		virtual	real GetPackingEfficency(  ) = 0;

		/**
		 * Set the automatic_texture_size flag. If true, then texture sizes will be computed 
		 *  for the reduced mesh depending on its pixel size on screen. 
		 * @param value the desired flag 
		 */
		virtual	void SetUseAutomaticTextureSize( bool value ) = 0;

		/**
		 * Set the automatic_texture_size flag. If true, then texture sizes will be computed 
		 *  for the reduced mesh depending on its pixel size on screen. 
		 * @return the current flag 
		 */
		virtual	bool GetUseAutomaticTextureSize(  ) = 0;

		/**
		 * Set a texture dimension length multiplier for the automatic texture size. 
		 * @param value the desired multiplier 
		 */
		virtual	void SetAutomaticTextureSizeMultiplier( real value ) = 0;

		/**
		 * Get the current automatic texture size multiplier. 
		 * @return the current multiplier 
		 */
		virtual	real GetAutomaticTextureSizeMultiplier(  ) = 0;

		/**
		 * If automatic_texture_size is enabled, then force the texture sizes to be a 
		 * power of 2 
		 * @param value the desired flag 
		 */
		virtual	void SetForcePower2Texture( bool value ) = 0;

		/**
		 * If automatic_texture_size is enabled, then force the texture sizes to be a 
		 * power of 2 
		 * @return the current flag 
		 */
		virtual	bool GetForcePower2Texture(  ) = 0;

		/**
		 * If UseVertexWeights is enabled, the weights help determine the UV charts and 
		 * their UV area. 
		 * @param value the desired flag 
		 */
		virtual	void SetUseVertexWeights( bool value ) = 0;

		/**
		 * If UseVertexWeights is enabled, the weights help determine the UV charts and 
		 * their UV area. 
		 * @return the current flag 
		 */
		virtual	bool GetUseVertexWeights(  ) = 0;

		/**
		 * If UseVisibilityWeights is enabled, the weights help determine the UV charts 
		 * and their UV area. 
		 * @param value the desired flag 
		 */
		virtual	void SetUseVisibilityWeights( bool value ) = 0;

		/**
		 * If UseVisibilityWeights is enabled, the weights help determine the UV charts 
		 * and their UV area. 
		 * @return the current flag 
		 */
		virtual	bool GetUseVisibilityWeights(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IParameterizer
	 */
	typedef CountedPointer<IParameterizer> spParameterizer;

	/// Conditional build define for the interface IParameterizer
	#define InterfaceDefined_SimplygonSDK_IParameterizer 1

	class IObject;

	/**
	 * IObjectCollection and its specializations handles a collection of IObjects. 
	 * There are methods for adding, removing and iterating through the objects. 
	 */
	class IObjectCollection : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IObjectCollection is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IObjectCollection",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IObjectCollection pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IObjectCollection pointer
		 * @return a pointer to the IObjectCollection object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IObjectCollection *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IObjectCollection") )
				return static_cast<IObjectCollection*>(ptr);
			return NULL;
			}

		/**
		 * Adds an object to the collection. The handle refers to the object until the 
		 * object is removed from the collection again. 
		 * @param obj is the object that is to be added to the collection 
		 * @return the handle of the added object in the collection 
		 */
		virtual	rhandle AddObject( IObject *obj ) = 0;

		/**
		 * Adds an object to the collection, as AddObject, but sorts the object into the 
		 * collection based on its name. 
		 * @param obj is the object that is to be added to the collection 
		 * @param ascending determines the sorting order 
		 * @return the handle of the added object in the collection 
		 */
		virtual	rhandle AddObjectSorted( IObject *obj , bool ascending ) = 0;

		/**
		 * Returns the start of the collection. The handle returned refers to the first 
		 * object in the collection. If the handle is NULL, then the collection is empty. 
		 * @return the first item in collection, or NULL if collection is empty 
		 */
		virtual	rhandle GetFirstItem(  ) = 0;

		/**
		 * Returns the handle to the next item in the collection. Use GetFirstItem and 
		 * this method to step through the collection. If the return is NULL, the end 
		 * of the collection has been reached. 
		 * @param objhandle is the handle of an object i 
		 * @return the handle of the object i+1 
		 */
		virtual	rhandle GetNextItem( rhandle objhandle ) = 0;

		/**
		 * Gets the object the handle is referring to. 
		 * @param objhandle is the handle of an object in the collection 
		 * @return the object associated with the handle 
		 */
		virtual	CountedPointer<IObject> GetItemsObject( rhandle objhandle ) = 0;

		/**
		 * Removes an item from the collection. The handle returned is the first item 
		 * after this item that is in the collection. If the return is NULL, the item 
		 * was the last item in the collection. Warning! The handle is invalid after the 
		 * removal of the object. 
		 * @param objhandle is the handle of the object that is to be removed 
		 * @return the handle to the item following the removed one, or NULL if collection 
		 * is now empty 
		 */
		virtual	rhandle RemoveItem( rhandle objhandle ) = 0;

		/**
		 * Removes an object from the collection. Only the first occurance of the object 
		 * is removed from the collection. 
		 * @param pobj is the object that is to be removed 
		 */
		virtual	void RemoveObject( IObject *pobj ) = 0;

		/**
		 * Removes all items from the collection. 
		 */
		virtual	void RemoveAllItems(  ) = 0;

		/**
		 * Get the number of items in the collection. 
		 * @return the number of items in the collection 
		 */
		virtual	unsigned int GetItemCount(  ) = 0;

		/**
		 * Returns the handle of the i:th item 
		 * @param index is the index of an object in the collection 
		 * @return the handle of the object with the relevant index 
		 */
		virtual	rhandle GetItem( unsigned int index ) = 0;

		/**
		 * Returns the object of the i:th item 
		 * @param index is the index of an object in the collection 
		 * @return the object with the relevant index 
		 */
		virtual	CountedPointer<IObject> GetItemAsObject( unsigned int index ) = 0;

		/**
		 * Returns true if the object is in the collection 
		 * @param pobj is the object to be checked for 
		 * @return true if the collection contains pobj, false otherwise 
		 */
		virtual	bool IsObjectInCollection( IObject *pobj ) = 0;

		/**
		 * Returns true if the item is in the collection 
		 * @param objhandle is the handle of an object to be checked for 
		 * @return true if the collection contains the object, false otherwise 
		 */
		virtual	bool IsItemInCollection( rhandle objhandle ) = 0;

		/**
		 * Finds a named object in the collection. The first object with this name is 
		 * returned. The search is case-sensitive. 
		 * @param name is the name of an object in the collection 
		 * @return the object with the relevant name, or NULL if none is found 
		 */
		virtual	CountedPointer<IObject> FindObject( const char * name ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IObjectCollection
	 */
	typedef CountedPointer<IObjectCollection> spObjectCollection;

	/// Conditional build define for the interface IObjectCollection
	#define InterfaceDefined_SimplygonSDK_IObjectCollection 1

	class IGeometryData;

	/**
	 * IGeometryDataCollection holds a number of IGeometryData objects. The objects 
	 * can be looked up by their names (if they have been named through IObject::SetName() 
	 *  ). 
	 */
	class IGeometryDataCollection : public IObjectCollection
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IGeometryDataCollection is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IGeometryDataCollection",type)==0 )
				return true;
			return IObjectCollection::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IGeometryDataCollection pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IGeometryDataCollection pointer
		 * @return a pointer to the IGeometryDataCollection object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IGeometryDataCollection *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IGeometryDataCollection") )
				return static_cast<IGeometryDataCollection*>(ptr);
			return NULL;
			}

		/**
		 * Adds a IGeometryData object to the collection. The handle refers to the ReGeometryData 
		 *  until it is removed from the collection again. If the ReGeometryData and the 
		 * pre-existing collection shares a field name they have to be of the same type 
		 * and tuple size. 
		 * @param obj is the IGeometryData to add 
		 * @return a handle to the added geometry in the collection 
		 */
		virtual	rhandle AddGeometryData( IGeometryData *obj ) = 0;

		/**
		 * Gets the object the handle is referring to. 
		 * @param objhandle the handle of the geometry data object to fetch 
		 * @return the requested IGeometryData object 
		 */
		virtual	CountedPointer<IGeometryData> GetGeometryData( rhandle objhandle ) = 0;

		/**
		 * Removes an object from the collection. Only the first occurrence of the object 
		 * is removed. 
		 * @param pobj the IGeometryData to remove from the collection 
		 */
		virtual	void RemoveGeometryData( IGeometryData *pobj ) = 0;

		/**
		 * Finds a named object. 
		 * @param name is the name of the geometry object 
		 * @return the requested IGeometryData object 
		 */
		virtual	CountedPointer<IGeometryData> FindGeometryData( const char * name ) = 0;

		/**
		 * Returns the object of the next item. 
		 * @param phandle_rhandleInputDataPtr is the handle to the geometry object 
		 */
		virtual	CountedPointer<IGeometryData> GetNextItemAsGeometryData( rhandle *phandle_rhandleInputDataPtr ) = 0;

		/**
		 * Returns true if the object is in the collection 
		 * @param pobj the GeometryData to look for 
		 * @return true if the GeometryData is in the collection, otherwise false 
		 */
		virtual	bool IsGeometryDataInCollection( IGeometryData *pobj ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IGeometryDataCollection
	 */
	typedef CountedPointer<IGeometryDataCollection> spGeometryDataCollection;

	/// Conditional build define for the interface IGeometryDataCollection
	#define InterfaceDefined_SimplygonSDK_IGeometryDataCollection 1

	class IScene;

	/**
	 * base class for graphics import classes 
	 */
	class IGraphicsImporter : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IGraphicsImporter is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IGraphicsImporter",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IGraphicsImporter pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IGraphicsImporter pointer
		 * @return a pointer to the IGraphicsImporter object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IGraphicsImporter *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IGraphicsImporter") )
				return static_cast<IGraphicsImporter*>(ptr);
			return NULL;
			}

		/**
		 * Set the main import file path. This must always be set. Some importers, such 
		 * as IWavefrontImporter, also have secondary files that may be set. For info 
		 * regarding these secondary files, please see the documentation for the specific 
		 * importer. 
		 * @param value the desired ImportFilePath string 
		 */
		virtual	void SetImportFilePath( const char * value ) = 0;

		/**
		 * Get the main import file path. This must always be set. Some importers, such 
		 * as IWavefrontImporter, also have secondary files that may be set. For info 
		 * regarding these secondary files, please see the documentation for the specific 
		 * importer. 
		 * @return the current ImportFilePath string 
		 */
		virtual	rstring GetImportFilePath(  ) = 0;

		/**
		 * Runs the import. Note that all parameters must be setup before importing. 
		 * @return true if the import was successful, false otherwise 
		 */
		virtual	bool RunImport(  ) = 0;

		/**
		 * Close any open file or stream, release any allocated data . 
		 */
		virtual	void Clear(  ) = 0;

		/**
		 * The the scene loaded from the importer. 
		 * @return the loaded scene 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IGraphicsImporter
	 */
	typedef CountedPointer<IGraphicsImporter> spGraphicsImporter;

	/// Conditional build define for the interface IGraphicsImporter
	#define InterfaceDefined_SimplygonSDK_IGraphicsImporter 1


	/**
	 * base class for graphics import classes 
	 */
	class IWavefrontImporter : public IGraphicsImporter
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IWavefrontImporter is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IWavefrontImporter",type)==0 )
				return true;
			return IGraphicsImporter::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IWavefrontImporter pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IWavefrontImporter pointer
		 * @return a pointer to the IWavefrontImporter object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IWavefrontImporter *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IWavefrontImporter") )
				return static_cast<IWavefrontImporter*>(ptr);
			return NULL;
			}

		/**
		 * Set the material file path. If the path is not set, it is assumed to equal 
		 * the path of the main obj file, but with the extension '.mtl' instead of '.obj'. 
		 * If no material file is found or specified, materials that are found in the 
		 * Wavefront file is added using default material settings to the material table. 
		 * @param value is the path to the material file 
		 */
		virtual	void SetMaterialFilePath( const char * value ) = 0;

		/**
		 * Get the material file path. If the path is not set, it is assumed to equal 
		 * the path of the main obj file, but with the extension '.mtl' instead of '.obj'. 
		 * If no material file is found or specified, materials that are found in the 
		 * Wavefront file is added using default material settings to the material table. 
		 * @return the path to the material file 
		 */
		virtual	rstring GetMaterialFilePath(  ) = 0;

		/**
		 * Set the ExtractGroups boolean. If true, each group within the wavefront file 
		 * is extracted into a separate geometry data object, that is named as the group. 
		 * If false, the whole file is imported into one geometry data, and groups are 
		 * ignored. 
		 * @param value is the bool the parameter is to be set to 
		 */
		virtual	void SetExtractGroups( bool value ) = 0;

		/**
		 * Get the ExtractGroups boolean. If true, each group within the wavefront file 
		 * is extracted into a separate geometry data object, that is named as the group. 
		 * If false, the whole file is imported into one geometry data, and groups are 
		 * ignored. 
		 * @return the current value of the ExtractGroups bool 
		 */
		virtual	bool GetExtractGroups(  ) = 0;

		/**
		 * Set the UseAlphaAsOpacity boolean. If set, then if the material has a map_d 
		 * texture definition, and that texture is RGBA, the material will use the alpha 
		 * channel rather than the color (R) to specify the opacity. 
		 * @param value is the bool the parameter is to be set to 
		 */
		virtual	void SetUseAlphaAsOpacity( bool value ) = 0;

		/**
		 * Get the AlphaAsOpacity boolean. If set, then if the material has a map_d texture 
		 *  definition, and that texture is RGBA, the material will use the alpha channel 
		 * rather than the color (R) to specify the opacity. 
		 * @return the current value of the AlphaAsOpacity bool 
		 */
		virtual	bool GetUseAlphaAsOpacity(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IWavefrontImporter
	 */
	typedef CountedPointer<IWavefrontImporter> spWavefrontImporter;

	/// Conditional build define for the interface IWavefrontImporter
	#define InterfaceDefined_SimplygonSDK_IWavefrontImporter 1

	class IScene;

	/**
	 * base class for graphics export classes 
	 */
	class IGraphicsExporter : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IGraphicsExporter is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IGraphicsExporter",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IGraphicsExporter pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IGraphicsExporter pointer
		 * @return a pointer to the IGraphicsExporter object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IGraphicsExporter *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IGraphicsExporter") )
				return static_cast<IGraphicsExporter*>(ptr);
			return NULL;
			}

		/**
		 * Set the main export file path. This is the main file being written to, and 
		 * must always be set. 
		 * @param value the desired file path 
		 */
		virtual	void SetExportFilePath( const char * value ) = 0;

		/**
		 * Get the main export file path. This is the main file being written to, and 
		 * must always be set. 
		 * @return the current file path 
		 */
		virtual	rstring GetExportFilePath(  ) = 0;

		/**
		 * Sets the scene to export. 
		 * @param scene the desired scene object 
		 */
		virtual	void SetScene( IScene *scene ) = 0;

		/**
		 * Sets the selection set (of nodes from the scene) to export. 
		 * @param exportSelectionSetID the desired selection set id to use 
		 */
		virtual	void SetSelectionSet( int exportSelectionSetID ) = 0;

		/**
		 * Runs the export. Note that all required parameters must be setup before exporting. 
		 * @return true if the export was successful, false otherwise 
		 */
		virtual	bool RunExport(  ) = 0;

		/**
		 * Sets the name of the texcoord level to use as primary level in case the file 
		 * format only supports a single texture coordinate layer. If the primary level 
		 * name is not set, the first found level will be used as primary level. 
		 * @param name the name of the primary texcoord level 
		 */
		virtual	void SetPrimaryTexCoordLevelName( const char * name ) = 0;

		/**
		 * Gets the name of the texcoord level used as primary level in case the file 
		 * format only supports a single texture coordinate layer 
		 * @return the current name of the texcoord level used as primary level 
		 */
		virtual	rstring GetPrimaryTexCoordLevelName(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IGraphicsExporter
	 */
	typedef CountedPointer<IGraphicsExporter> spGraphicsExporter;

	/// Conditional build define for the interface IGraphicsExporter
	#define InterfaceDefined_SimplygonSDK_IGraphicsExporter 1


	/**
	 * Wavefront .obj geometry exporter 
	 */
	class IWavefrontExporter : public IGraphicsExporter
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IWavefrontExporter is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IWavefrontExporter",type)==0 )
				return true;
			return IGraphicsExporter::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IWavefrontExporter pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IWavefrontExporter pointer
		 * @return a pointer to the IWavefrontExporter object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IWavefrontExporter *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IWavefrontExporter") )
				return static_cast<IWavefrontExporter*>(ptr);
			return NULL;
			}

		/**
		 * Set the material file path. If the path is not set, it is assumed to equal 
		 * the path of the main obj file, but with the extension '.mtl' instead of '.obj'. 
		 * @param value is the path to the material file 
		 */
		virtual	void SetMaterialFilePath( const char * value ) = 0;

		/**
		 * Get the material file path. If the path is not set, it is assumed to equal 
		 * the path of the main obj file, but with the extension '.mtl' instead of '.obj'. 
		 * @return the path to the material file 
		 */
		virtual	rstring GetMaterialFilePath(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IWavefrontExporter
	 */
	typedef CountedPointer<IWavefrontExporter> spWavefrontExporter;

	/// Conditional build define for the interface IWavefrontExporter
	#define InterfaceDefined_SimplygonSDK_IWavefrontExporter 1


	/**
	 * IBinaryImporter loads the geometry data from a binary file, that is specific 
	 * for Simplygon. Please note that the binary file is only intended for temporary 
	 * storage, and that no compatibility between versions of Simplygon is guaranteed. 
	 *  
	 */
	class IBinaryImporter : public IGraphicsImporter
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IBinaryImporter is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IBinaryImporter",type)==0 )
				return true;
			return IGraphicsImporter::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IBinaryImporter pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IBinaryImporter pointer
		 * @return a pointer to the IBinaryImporter object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IBinaryImporter *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IBinaryImporter") )
				return static_cast<IBinaryImporter*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IBinaryImporter
	 */
	typedef CountedPointer<IBinaryImporter> spBinaryImporter;

	/// Conditional build define for the interface IBinaryImporter
	#define InterfaceDefined_SimplygonSDK_IBinaryImporter 1


	/**
	 * IBinaryExporter stores the geometry data into a binary file, that is specific 
	 * for Simplygon. Please note that the binary file is only intended for temporary 
	 * storage, and that no compatibility between versions of Simplygon is guaranteed. 
	 *  
	 */
	class IBinaryExporter : public IGraphicsExporter
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IBinaryExporter is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IBinaryExporter",type)==0 )
				return true;
			return IGraphicsExporter::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IBinaryExporter pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IBinaryExporter pointer
		 * @return a pointer to the IBinaryExporter object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IBinaryExporter *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IBinaryExporter") )
				return static_cast<IBinaryExporter*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IBinaryExporter
	 */
	typedef CountedPointer<IBinaryExporter> spBinaryExporter;

	/// Conditional build define for the interface IBinaryExporter
	#define InterfaceDefined_SimplygonSDK_IBinaryExporter 1

	class IGeometryData;

	/**
	 * Checks the geometry for consistent indices, number of triangles etc. 
	 */
	class IGeometryValidator : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IGeometryValidator is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IGeometryValidator",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IGeometryValidator pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IGeometryValidator pointer
		 * @return a pointer to the IGeometryValidator object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IGeometryValidator *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IGeometryValidator") )
				return static_cast<IGeometryValidator*>(ptr);
			return NULL;
			}

		/**
		 * Checks if the data stored in the Geometry is stored correctly. 
		 * @param geom the geometry data to validate 
		 * @return True if the geometry is stored correct, or false if something is wrong. 
		 *  
		 */
		virtual	bool ValidateGeometry( IGeometryData *geom ) = 0;

		/**
		 * Returns the error-code for the last check that was performed. 0 means no error 
		 * was found, and values less than 0 means an error was found. 
		 * @return the last error-code 
		 */
		virtual	unsigned int GetErrorValue(  ) = 0;

		/**
		 * Returns a string describing the error that was found for the last check that 
		 * was performed. 
		 * @return the last string describing the error 
		 */
		virtual	rstring GetErrorString(  ) = 0;

		/**
		 * Set the CheckDegenerateFaceIndices flag. If set, faces that have the same index 
		 *  more than once will not be allowed. 
		 * @param value the desired flag value 
		 */
		virtual	void SetCheckDegenerateFaceIndices( bool value ) = 0;

		/**
		 * Get the CheckDegenerateFaceIndices flag. If set, faces that have the same index 
		 *  more than once will not be allowed. 
		 * @return the flag value 
		 */
		virtual	bool GetCheckDegenerateFaceIndices(  ) = 0;

		/**
		 * Set the CheckZeroAreaFaces flag. If set, faces that have zero area will not 
		 * be allowed. 
		 * @param value the desired flag value 
		 */
		virtual	void SetCheckZeroAreaFaces( bool value ) = 0;

		/**
		 * Get the CheckZeroAreaFaces flag. If set, faces that have zero area will not 
		 * be allowed. 
		 * @return the flag value 
		 */
		virtual	bool GetCheckZeroAreaFaces(  ) = 0;

		/**
		 * Get the AllowedMaterialCount count. This is the number of materials used in 
		 * the scene. Only indices within this range are allowed. 
		 * @param value the desired flag value 
		 */
		virtual	void SetAllowedMaterialCount( unsigned int value ) = 0;

		/**
		 * Get the AllowedBoneCount count. This is the number of bones used in the scene. 
		 * Only indices within this range are allowed. 
		 * @param value the desired flag value 
		 * @return the flag value 
		 */
		virtual	void SetAllowedBoneCount( unsigned int value ) = 0;

		/**
		 * Get the AllowedGroupCount count. This is the number of groups/objects used 
		 * in the scene. Only indices within this range are allowed. 
		 * @param value the desired flag value 
		 * @return the flag value 
		 */
		virtual	void SetAllowedGroupCount( unsigned int value ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IGeometryValidator
	 */
	typedef CountedPointer<IGeometryValidator> spGeometryValidator;

	/// Conditional build define for the interface IGeometryValidator
	#define InterfaceDefined_SimplygonSDK_IGeometryValidator 1


	/**
	 * ISettingsObject is the abstract base class for all settings objects in Simplygon. 
	 *  Settings objects contains parameters for processing objects to use. These settings 
	 *  objects will have their data set and reached from a IProcessingObject and then 
	 * used therein. 
	 */
	class ISettingsObject : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISettingsObject is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISettingsObject",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISettingsObject pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISettingsObject pointer
		 * @return a pointer to the ISettingsObject object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISettingsObject *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISettingsObject") )
				return static_cast<ISettingsObject*>(ptr);
			return NULL;
			}

		/**
		 * Validates the current settings values. 
		 * @return true if all settings are valid 
		 */
		virtual	bool ValidateSettings(  ) = 0;

		/**
		 * Returns a string describing the error that was found the last time ValidateSettings 
		 *  was performed. 
		 * @return the last string describing the error 
		 */
		virtual	rstring GetErrorString(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISettingsObject
	 */
	typedef CountedPointer<ISettingsObject> spSettingsObject;

	/// Conditional build define for the interface ISettingsObject
	#define InterfaceDefined_SimplygonSDK_ISettingsObject 1

	class IMaterialCasterSettings;
	class ITextureTable;
	class IMaterialTable;
	class IMappingImage;
	class IImageData;

	/**
	 * IMaterialCaster is the base interface for material casting interfaces. The interface 
	 *  exports methods for defining input geometries & mapping image, materials, output 
	 *  image paths & formats 
	 */
	class IMaterialCaster : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IMaterialCaster is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IMaterialCaster",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IMaterialCaster pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IMaterialCaster pointer
		 * @return a pointer to the IMaterialCaster object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IMaterialCaster *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IMaterialCaster") )
				return static_cast<IMaterialCaster*>(ptr);
			return NULL;
			}

		/**
		 * Get the settings object 
		 */
		virtual	CountedPointer<IMaterialCasterSettings> GetCasterSettings(  ) = 0;

		/**
		 * Set the SourceTextures object. The SourceTextures object contains all textures 
		 * of the the source geometry. 
		 * @param value is the texture table to which SourceTextures will be set 
		 */
		virtual	void SetSourceTextures( ITextureTable *value ) = 0;

		/**
		 * Get the SourceTextures object. The SourceTextures object contains all textures 
		 * of the the source geometry. 
		 * @return the current SourceTextures material table 
		 */
		virtual	CountedPointer<ITextureTable> GetSourceTextures(  ) = 0;

		/**
		 * Set the SourceMaterials object. The SourceMaterials object contains all materials 
		 *  of the the source geometry. The source geometry must have a "MaterialIds" 
		 * field that indices the material table. 
		 * @param value is the material table to which SourceMaterials will be set 
		 */
		virtual	void SetSourceMaterials( IMaterialTable *value ) = 0;

		/**
		 * Get the SourceMaterials object. The SourceMaterials object contains all materials 
		 *  of the the source geometry. The source geometry must have a "MaterialIds" 
		 * field that indices the material table. 
		 * @return the current SourceMaterials material table 
		 */
		virtual	CountedPointer<IMaterialTable> GetSourceMaterials(  ) = 0;

		/**
		 * Set the DestMaterialId object. If set, only the parts of the destination map 
		 * that. To disable, set to -1. 
		 * @param value is the value to which DestMaterialId will be set 
		 */
		virtual	void SetDestMaterialId( rid value ) = 0;

		/**
		 * Get the DestMaterialId object. If set, only the parts of the destination map 
		 * that. To disable, set to -1. 
		 * @return the current value of DestMaterialId 
		 */
		virtual	rid GetDestMaterialId(  ) = 0;

		/**
		 * Set the MappingImage object. The MappingImage object contains the mapping between 
		 *  the Geometry and SourceGeometry objects. 
		 * @param value is the mapping image to which MappingImage will be set 
		 */
		virtual	void SetMappingImage( IMappingImage *value ) = 0;

		/**
		 * Get the MappingImage object. The MappingImage object contains the mapping between 
		 *  the Geometry and SourceGeometry objects. 
		 * @return the current MappingImage 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * Set the Dilation value. Where applicable, such as colors and normals, the caster 
		 *  will fill empty pixels surrounding filled pixels with values mixed from the 
		 * filled ones. This setting sets how many pixels to fill outside the original 
		 * filled pixels. 
		 * @param Dilation is the new value of Dilation 
		 */
		virtual	void SetDilation( unsigned int Dilation ) = 0;

		/**
		 * Get the Dilation value. Where applicable, such as colors and normals, the caster 
		 *  will fill empty pixels surrounding filled pixels with values mixed from the 
		 * filled ones. This setting sets how many pixels to fill outside the original 
		 * filled pixels. 
		 * @return the current value of Dilation 
		 */
		virtual	unsigned int GetDilation(  ) = 0;

		/**
		 * Set the OutputChannels parameter. This can be set to either: <ul><li>1 (Luminance)</li><li>3 
		 *  (RGB)</li><li>4 (RGBA)</li></ul> 
		 * @param OutputChannels is the number of OutputChannels the texture will contain. 
		 *  
		 */
		virtual	void SetOutputChannels( unsigned int OutputChannels ) = 0;

		/**
		 * Get the OutputChannels parameter. This can be set to either: <ul><li>1 (Luminance)</li><li>3 
		 *  (RGB)</li><li>4 (RGBA)</li></ul> 
		 * @return the current value of OutputChannels 
		 */
		virtual	unsigned int GetOutputChannels(  ) = 0;

		/**
		 * Set the OutputBitDepth parameter. This can be either 8 or 16. Please note that 
		 * this is the bit depth per-channel, so a bit depth of 8 and a OutputChannels 
		 * value of 3 will result in 8*3 = 24-bit RGB values. 
		 * @param OutputChannelBitDepth is the bit depth of the OutputChannel (8 or 16) 
		 *  
		 */
		virtual	void SetOutputChannelBitDepth( unsigned int OutputChannelBitDepth ) = 0;

		/**
		 * Get the OutputBitDepth parameter. This can be either 8 or 16. Please note that 
		 * this is the bit depth per-channel, so a bit depth of 8 and a OutputChannels 
		 * value of 3 will result in 8*3 = 24-bit RGB values. 
		 * @return the current value of OutputBitDepth 
		 */
		virtual	unsigned int GetOutputChannelBitDepth(  ) = 0;

		/**
		 * Set the OutputFilePath file path, where the output image will be placed. Note 
		 * that the extension specified will specify the file format. Please note that 
		 * only PNG (extension .png) will be able to support all possible bit dephts (such 
		 * as RGB 16-bit per channel). Either OutputImage or OutputFilePath must be set. 
		 * If OutputImage is set, then OutputFilePath is ignored. 
		 * @param value is the string to which OutputFilePath will be set 
		 */
		virtual	void SetOutputFilePath( const char * value ) = 0;

		/**
		 * Get the OutputFilePath file path, where the output image will be placed. Note 
		 * that the extension specified will specify the file format. Please note that 
		 * only PNG (extension .png) will be able to support all possible bit dephts (such 
		 * as RGB 16-bit per channel). Either OutputImage or OutputFilePath must be set. 
		 * If OutputImage is set, then OutputFilePath is ignored. 
		 * @return the current OutputFilePath 
		 */
		virtual	rstring GetOutputFilePath(  ) = 0;

		/**
		 * Set the OutputImage object that will receive the image. The current contents 
		 * of the image will be removed, and the image will be written to the Colors field 
		 * of the ReImageData object. Either OutputImage or OutputFilePath must be set. 
		 * If OutputImage is set, then OutputFilePath is ignored. 
		 * @param value is the image data to which OutputImage will be set 
		 */
		virtual	void SetOutputImage( IImageData *value ) = 0;

		/**
		 * Get the OutputImage object that will receive the image. The current contents 
		 * of the image will be removed, and the image will be written to the Colors field 
		 * of the ReImageData object. Either OutputImage or OutputFilePath must be set. 
		 * If OutputImage is set, then OutputFilePath is ignored. 
		 * @return the current OutputImage 
		 */
		virtual	CountedPointer<IImageData> GetOutputImage(  ) = 0;

		/**
		 * The type of dithering to use when creating the output object. Use the DitherPattern 
		 *  enum to choose. Default is SG_DITHERPATTERNS_NO_DITHER. <ul><li>0 (SG_DITHERPATTERNS_NO_DITHER)</li> 
		 * <li>1 (SG_DITHERPATTERNS_FLOYDSTEINBERG)</li><li>2 (SG_DITHERPATTERNS_JARVISJUDICENINKE)</li> 
		 * <li>3 (SG_DITHERPATTERNS_SIERRA)</li></ul> 
		 * @param DitherType is the value of the DitherType to use 
		 */
		virtual	void SetDitherType( unsigned int DitherType ) = 0;

		/**
		 * The type of dithering to use when creating the output object. Default is SG_DITHERPATTERNS_NO_DITHER. 
		 *  
		 */
		virtual	unsigned int GetDitherType(  ) = 0;

		/**
		 * Set the fill mode <ul><li>0:SG_ATLASFILLMODE_INTERPOLATE</li><li>1:SG_ATLASFILLMODE_NEARESTNEIGHBOR</li><li>2:SG_ATLASFILLMODE_NONE</li></ul> 
		 * @param FillMode is the value of the FillMode to use 
		 */
		virtual	void SetFillMode( unsigned int FillMode ) = 0;

		/**
		 * Get the fill mode 
		 * @return the current fill mode 
		 */
		virtual	unsigned int GetFillMode(  ) = 0;

		/**
		 * Set the UseMultisampling 
		 * @param UseMultisampling the desired value of the flag 
		 */
		virtual	void SetUseMultisampling( bool UseMultisampling ) = 0;

		/**
		 * Get the multisampling flag 
		 * @return the current multisampling flag 
		 */
		virtual	bool GetUseMultisampling(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RunProcessing RunProcessing() \endlink instead. Runs the material 
		 * casting from SourceGeometry to Geometry. 
		 */
		virtual	SGDEPRECATED void CastMaterials(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IMaterialCaster
	 */
	typedef CountedPointer<IMaterialCaster> spMaterialCaster;

	/// Conditional build define for the interface IMaterialCaster
	#define InterfaceDefined_SimplygonSDK_IMaterialCaster 1


	/**
	 * IMaterialCasterSettings is the base interface for material casting settings 
	 *  
	 */
	class IMaterialCasterSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IMaterialCasterSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IMaterialCasterSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IMaterialCasterSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IMaterialCasterSettings pointer
		 * @return a pointer to the IMaterialCasterSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IMaterialCasterSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IMaterialCasterSettings") )
				return static_cast<IMaterialCasterSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set which material channel to cast. The material channels are defined as SG_MATERIAL_CHANNEL_[ 
		 *  CHANNEL ]. For example, the diffuse channel is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @param value the desired material channel 
		 */
		virtual	void SetMaterialChannel( const char * value ) = 0;

		/**
		 * Get which material channel to cast. The material channels are defined as SG_MATERIAL_CHANNEL_[ 
		 *  CHANNEL ]. For example, the diffuse channel is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @return the current material channel. 
		 */
		virtual	rstring GetMaterialChannel(  ) = 0;

		/**
		 * Set the Dilation value. Where applicable, such as colors and normals, the caster 
		 *  will fill empty pixels surrounding filled pixels with values mixed from the 
		 * filled ones. This setting sets how many pixels to fill outside the original 
		 * filled pixels. 
		 * @param value is the value to which Dilation will be set 
		 */
		virtual	void SetDilation( unsigned int value ) = 0;

		/**
		 * Get the Dilation value. Where applicable, such as colors and normals, the caster 
		 *  will fill empty pixels surrounding filled pixels with values mixed from the 
		 * filled ones. This setting sets how many pixels to fill outside the original 
		 * filled pixels. 
		 * @return the Dilation value 
		 */
		virtual	unsigned int GetDilation(  ) = 0;

		/**
		 * Set the OutputChannels parameter. This can be set to either: <ul><li>1 (Luminance)</li><li>3 
		 *  (RGB)</li><li>4 (RGBA)</li></ul> 
		 * @param value is the value to which OutputChannels will be set 
		 */
		virtual	void SetOutputChannels( unsigned int value ) = 0;

		/**
		 * Get the OutputChannels parameter. 
		 * @return the OutputChannels value 
		 */
		virtual	unsigned int GetOutputChannels(  ) = 0;

		/**
		 * Set the OutputBitDepth parameter. This can be either 8 or 16. Please note that 
		 * this is the bit depth per-channel, so a bit depth of 8 and a OutputChannels 
		 * value of 3 will result in 8*3 = 24-bit RGB values. 
		 * @param value is the value to which OutputBitDepth will be set 
		 */
		virtual	void SetOutputChannelBitDepth( unsigned int value ) = 0;

		/**
		 * Get the OutputBitDepth parameter. This can be either 8 or 16. Please note that 
		 * this is the bit depth per-channel, so a bit depth of 8 and a OutputChannels 
		 * value of 3 will result in 8*3 = 24-bit RGB values. 
		 * @return the OutputBitDepth value 
		 */
		virtual	unsigned int GetOutputChannelBitDepth(  ) = 0;

		/**
		 * The type of dithering to use when creating the output object. Use the DitherPattern 
		 *  enum to choose. Default is SG_DITHERPATTERNS_NO_DITHER. <ul><li>0 (SG_DITHERPATTERNS_NO_DITHER)</li> 
		 * <li>1 (SG_DITHERPATTERNS_FLOYDSTEINBERG)</li><li>2 (SG_DITHERPATTERNS_JARVISJUDICENINKE)</li> 
		 * <li>3 (SG_DITHERPATTERNS_SIERRA)</li></ul> 
		 * @param value is the value of the DitherType to use 
		 */
		virtual	void SetDitherType( unsigned int value ) = 0;

		/**
		 * The type of dithering to use when creating the output object. Use the DitherPattern 
		 *  enum to choose. Default is SG_DITHERPATTERNS_NO_DITHER. 
		 */
		virtual	unsigned int GetDitherType(  ) = 0;

		/**
		 * The fill mode determines what to do with the pixels that remain unfilled after 
		 * both the casting and dilation has been performed.<br> Options are: <br> SG_ATLASFILLMODE_NONE 
		 *  = Do not fill remaining pixels <br> SG_ATLASFILLMODE_INTERPOLATE = linearly 
		 * interpolate the closest samples <br> SG_ATLASFILLMODE_NEARESTNEIGHBOR = use 
		 * the closest available pixel value without interpolation <br> 
		 * @param value is the mode to which the fill mode will be set 
		 */
		virtual	void SetFillMode( int value ) = 0;

		/**
		 * The fill mode determines what to do with the pixels that remain unfilled after 
		 * both the casting and dilation has been performed.<br> 
		 * @return Fill mode value 
		 */
		virtual	int GetFillMode(  ) = 0;

		/**
		 * Enable or disable multisampling 
		 * @param value the desired value of the flag 
		 */
		virtual	void SetUseMultisampling( bool value ) = 0;

		/**
		 * Enable or disable multisampling 
		 */
		virtual	bool GetUseMultisampling(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IMaterialCasterSettings
	 */
	typedef CountedPointer<IMaterialCasterSettings> spMaterialCasterSettings;

	/// Conditional build define for the interface IMaterialCasterSettings
	#define InterfaceDefined_SimplygonSDK_IMaterialCasterSettings 1

	class IColorCasterSettings;

	/**
	 * IColorCaster is the interface used to cast basic color values. Currently supported 
	 *  material color value types are Ambient, Diffuse and Specular + Shininess. 
	 */
	class IColorCaster : public IMaterialCaster
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IColorCaster is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IColorCaster",type)==0 )
				return true;
			return IMaterialCaster::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IColorCaster pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IColorCaster pointer
		 * @return a pointer to the IColorCaster object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IColorCaster *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IColorCaster") )
				return static_cast<IColorCaster*>(ptr);
			return NULL;
			}

		/**
		 * Set which material channel to cast. The material channels are defined as SG_MATERIAL_CHANNEL_[ 
		 *  CHANNEL ]. Or a custom channel name. For example, the diffuse channel is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @param ColorType the name of the material channel 
		 */
		virtual	void SetColorType( const char * ColorType ) = 0;

		/**
		 * Get which material channel to cast. The material channels are defined as SG_MATERIAL_CHANNEL_[ 
		 *  CHANNEL ]. For example, the diffuse channel is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @return the current material channel. 
		 */
		virtual	rstring GetColorType(  ) = 0;

		/**
		 * Set the OutputSRGB flag. If set, output textures are exported in SRGB format. 
		 * Input texture format is determined by the flag in the corresponding texture 
		 * node in the material definition. Default is true. 
		 * @param OutputSRGB the desired flag value 
		 */
		virtual	void SetOutputSRGB( bool OutputSRGB ) = 0;

		/**
		 * Get the OutputSRGB flag. If set, output textures are exported in SRGB format. 
		 * Input texture format is determined by the flag in the corresponding texture 
		 * node in the material definition. Default is true. 
		 * @return the current IsSRGB flag 
		 */
		virtual	bool GetOutputSRGB(  ) = 0;

		/**
		 * Set the BakeOpacityInAlpha flag. If set, the total opacity of the baked layers 
		 * replaces the alpha value in the output texture. 
		 * @param BakeOpacityInAlpha the desired flag value 
		 */
		virtual	void SetBakeOpacityInAlpha( bool BakeOpacityInAlpha ) = 0;

		/**
		 * Get the BakeOpacityInAlpha flag. If set, the total opacity of the baked layers 
		 * replaces the alpha value in the output texture. 
		 * @return the current BakeOpacityInAlpha value 
		 */
		virtual	bool GetBakeOpacityInAlpha(  ) = 0;

		/**
		 * Set the SkipCastingIfNoInputChannel flag. If this flag is set, the caster will 
		 * only run if there is at least one input material that has the main input channel 
		 * which is to be casted. If the flag is not set, the caster will always produce 
		 * an output, even if it is totally empty/unset. 
		 * @param SkipCastingIfNoInputChannel the desired flag value 
		 */
		virtual	void SetSkipCastingIfNoInputChannel( bool SkipCastingIfNoInputChannel ) = 0;

		/**
		 * Get the SkipCastingIfNoInputChannel flag. If this flag is set, the caster will 
		 * only run if there is at least one input material that has the main input channel 
		 * which is to be casted. If the flag is not set, the caster will always produce 
		 * an output, even if it is totally empty/unset. 
		 * @return the current SkipCastingIfNoInputChannel flag 
		 */
		virtual	bool GetSkipCastingIfNoInputChannel(  ) = 0;

		/**
		 * Get the LastCastingSkipped flag. If SkipCastingIfNoInputChannel is set to true, 
		 *  and the last processing skipped, the LastCastingWasSkipped will be set to 
		 * true. The initial value before the first process will be set to false. 
		 */
		virtual	bool GetLastCastingWasSkipped(  ) = 0;

		/**
		 * Get the color caster settings object 
		 * @return Color caster settings object 
		 */
		virtual	CountedPointer<IColorCasterSettings> GetColorCasterSettings(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IColorCaster
	 */
	typedef CountedPointer<IColorCaster> spColorCaster;

	/// Conditional build define for the interface IColorCaster
	#define InterfaceDefined_SimplygonSDK_IColorCaster 1


	/**
	 * IColorCasterSettings is the interface for basic color caster settings 
	 */
	class IColorCasterSettings : public IMaterialCasterSettings
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IColorCasterSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IColorCasterSettings",type)==0 )
				return true;
			return IMaterialCasterSettings::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IColorCasterSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IColorCasterSettings pointer
		 * @return a pointer to the IColorCasterSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IColorCasterSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IColorCasterSettings") )
				return static_cast<IColorCasterSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the OutputSRGB flag. If set, output textures are exported in SRGB format. 
		 * Input texture format is determined by the flag in the corresponding texture 
		 * node in the material definition. Default is true. 
		 * @param value the desired IsSRGB flag 
		 */
		virtual	void SetOutputSRGB( bool value ) = 0;

		/**
		 * Get the OutputSRGB flag. If set, output textures are exported in SRGB format. 
		 * Input texture format is determined by the flag in the corresponding texture 
		 * node in the material definition. Default is true. 
		 * @return the current IsSRGB flag 
		 */
		virtual	bool GetOutputSRGB(  ) = 0;

		/**
		 * Set the BakeOpacityInAlpha flag. If set, the total opacity of the baked layers 
		 * replaces the alpha value in the output texture. 
		 * @param value the new BakeOpacityInAlpha value 
		 */
		virtual	void SetBakeOpacityInAlpha( bool value ) = 0;

		/**
		 * Get the BakeOpacityInAlpha flag. If set, the total opacity of the baked layers 
		 * replaces the alpha value in the output texture. 
		 * @return the current BakeOpacityInAlpha value 
		 */
		virtual	bool GetBakeOpacityInAlpha(  ) = 0;

		/**
		 * Set the SkipCastingIfNoInputChannel flag. If this flag is set, the caster will 
		 * only run if there is at least one input material that has the main input channel 
		 * which is to be casted. If the flag is not set, the caster will always produce 
		 * an output, even if it is totally empty/unset. 
		 * @param value the desired SkipCastingIfNoInputChannel flag 
		 */
		virtual	void SetSkipCastingIfNoInputChannel( bool value ) = 0;

		/**
		 * Get the SkipCastingIfNoInputChannel flag. If this flag is set, the caster will 
		 * only run if there is at least one input material that has the main input channel 
		 * which is to be casted. If the flag is not set, the caster will always produce 
		 * an output, even if it is totally empty/unset. 
		 * @return the current SkipCastingIfNoInputChannel flag 
		 */
		virtual	bool GetSkipCastingIfNoInputChannel(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IColorCasterSettings
	 */
	typedef CountedPointer<IColorCasterSettings> spColorCasterSettings;

	/// Conditional build define for the interface IColorCasterSettings
	#define InterfaceDefined_SimplygonSDK_IColorCasterSettings 1

	class IOpacityCasterSettings;

	/**
	 * IOpacityCaster is the interface used to cast opacity values. To be used when 
	 * opacity is needed as a separate texture. 
	 */
	class IOpacityCaster : public IMaterialCaster
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IOpacityCaster is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IOpacityCaster",type)==0 )
				return true;
			return IMaterialCaster::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IOpacityCaster pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IOpacityCaster pointer
		 * @return a pointer to the IOpacityCaster object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IOpacityCaster *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IOpacityCaster") )
				return static_cast<IOpacityCaster*>(ptr);
			return NULL;
			}

		/**
		 * Set which opacity channel to cast. The default value is SG_MATERIAL_CHANNEL_OPACITY. 
		 * @param ColorType the material channel name 
		 */
		virtual	void SetColorType( const char * ColorType ) = 0;

		/**
		 * Get which opacity channel to cast. The default value is SG_MATERIAL_CHANNEL_OPACITY. 
		 * @return the desired normals channel 
		 */
		virtual	rstring GetColorType(  ) = 0;

		/**
		 * Set the OutputSRGB flag. If set, output textures are exported in SRGB format. 
		 * Input texture format is determined by the flag in the corresponding texture 
		 * node in the material definition. Default is true. 
		 * @param OutputSRGB the desired flag value 
		 */
		virtual	void SetOutputSRGB( bool OutputSRGB ) = 0;

		/**
		 * Get the OutputSRGB flag. If set, output textures are exported in SRGB format. 
		 * Input texture format is determined by the flag in the corresponding texture 
		 * node in the material definition. Default is true. 
		 * @return the current IsSRGB flag 
		 */
		virtual	bool GetOutputSRGB(  ) = 0;

		/**
		 * Get the opacity caster settings object 
		 * @return Opacity caster settings object 
		 */
		virtual	CountedPointer<IOpacityCasterSettings> GetOpacityCasterSettings(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IOpacityCaster
	 */
	typedef CountedPointer<IOpacityCaster> spOpacityCaster;

	/// Conditional build define for the interface IOpacityCaster
	#define InterfaceDefined_SimplygonSDK_IOpacityCaster 1


	/**
	 * IOpacityCasterSettings is the interface used to manage settings for an opacity 
	 * caster 
	 */
	class IOpacityCasterSettings : public IMaterialCasterSettings
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IOpacityCasterSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IOpacityCasterSettings",type)==0 )
				return true;
			return IMaterialCasterSettings::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IOpacityCasterSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IOpacityCasterSettings pointer
		 * @return a pointer to the IOpacityCasterSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IOpacityCasterSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IOpacityCasterSettings") )
				return static_cast<IOpacityCasterSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the OutputSRGB flag. If set, output textures are exported in SRGB format. 
		 * Input texture format is determined by the flag in the corresponding texture 
		 * node in the material definition. Default is true. 
		 * @param value the desired IsSRGB flag 
		 */
		virtual	void SetOutputSRGB( bool value ) = 0;

		/**
		 * Get the OutputSRGB flag. If set, output textures are exported in SRGB format. 
		 * Input texture format is determined by the flag in the corresponding texture 
		 * node in the material definition. Default is true. 
		 * @return the current IsSRGB flag 
		 */
		virtual	bool GetOutputSRGB(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IOpacityCasterSettings
	 */
	typedef CountedPointer<IOpacityCasterSettings> spOpacityCasterSettings;

	/// Conditional build define for the interface IOpacityCasterSettings
	#define InterfaceDefined_SimplygonSDK_IOpacityCasterSettings 1

	class INormalCasterSettings;

	/**
	 * INormalCaster is the interface used to cast normals data onto the receiving 
	 * geometry. 
	 */
	class INormalCaster : public IMaterialCaster
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if INormalCaster is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("INormalCaster",type)==0 )
				return true;
			return IMaterialCaster::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a INormalCaster pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a INormalCaster pointer
		 * @return a pointer to the INormalCaster object, if the cast can be made, and a NULL pointer otherwise
		 */
		static INormalCaster *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("INormalCaster") )
				return static_cast<INormalCaster*>(ptr);
			return NULL;
			}

		/**
		 * Set which normals channel to cast. The default value is SG_MATERIAL_CHANNEL_NORMALS. 
		 * @param NormalsChannel the material channel name 
		 */
		virtual	void SetNormalsChannel( const char * NormalsChannel ) = 0;

		/**
		 * Get which normals channel to cast. The default value is SG_MATERIAL_CHANNEL_NORMALS. 
		 * @return the desired normals channel 
		 */
		virtual	rstring GetNormalsChannel(  ) = 0;

		/**
		 * Set the FlipBackfacingNormals flag. If set, then normals will be flipped if 
		 * they are back facing, i.e., pointing into the surface. This may introduce artifacts 
		 * on geometries that have correctly facing normals, so only use for geometries 
		 * with known back-facing normals. 
		 * @param FlipBackfacingNormals the desired flag value 
		 */
		virtual	void SetFlipBackfacingNormals( bool FlipBackfacingNormals ) = 0;

		/**
		 * Get the FlipBackfacingNormals flag. If set, then normals will be flipped if 
		 * they are back facing, i.e., pointing into the surface. This may introduce artifacts 
		 * on geometries that have correctly facing normals, so only use for geometries 
		 * with known back-facing normals. 
		 * @return the current value of the FlipBackfacingNormals bool 
		 */
		virtual	bool GetFlipBackfacingNormals(  ) = 0;

		/**
		 * Set the GenerateTangentSpaceNormals flag. If set, then the generated normal 
		 * map will be in tangent space. Please note that the geometry casted to must 
		 * contain tangent space fields. 
		 * @param GenerateTangentSpaceNormals the desired flag value 
		 */
		virtual	void SetGenerateTangentSpaceNormals( bool GenerateTangentSpaceNormals ) = 0;

		/**
		 * Get the GenerateTangentSpaceNormals flag. If set, then the generated normal 
		 * map will be in tangent space. Please note that the geometry casted to must 
		 * contain tangent space fields. 
		 * @return the current value of the GenerateTangentSpaceNormals bool 
		 */
		virtual	bool GetGenerateTangentSpaceNormals(  ) = 0;

		/**
		 * Set the FlipGreen flag. If set, the green channel is assumed to point along 
		 * the negative bitangent instead of the positive. 
		 * @param FlipGreen the desired flag value 
		 */
		virtual	void SetFlipGreen( bool FlipGreen ) = 0;

		/**
		 * Get the FlipGreen flag. If set, the green channel is assumed to point along 
		 * the negative bitangent instead of the positive. 
		 * @return the current value of the FlipGreen bool 
		 */
		virtual	bool GetFlipGreen(  ) = 0;

		/**
		 * Get the normal caster settings object 
		 * @return Normal caster settings object 
		 */
		virtual	CountedPointer<INormalCasterSettings> GetNormalCasterSettings(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an INormalCaster
	 */
	typedef CountedPointer<INormalCaster> spNormalCaster;

	/// Conditional build define for the interface INormalCaster
	#define InterfaceDefined_SimplygonSDK_INormalCaster 1


	/**
	 * INormalCasterSettings is the interface used to manage settings for a normal 
	 * caster 
	 */
	class INormalCasterSettings : public IMaterialCasterSettings
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if INormalCasterSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("INormalCasterSettings",type)==0 )
				return true;
			return IMaterialCasterSettings::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a INormalCasterSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a INormalCasterSettings pointer
		 * @return a pointer to the INormalCasterSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static INormalCasterSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("INormalCasterSettings") )
				return static_cast<INormalCasterSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the FlipBackfacingNormals flag. If set, then normals will be flipped if 
		 * they are back facing, i.e., pointing into the surface. This may introduce artifacts 
		 * on geometries that have correctly facing normals, so only use for geometries 
		 * with known back-facing normals. 
		 * @param value is the value to which the FlipBackfacingNormals bool is to be 
		 * set 
		 */
		virtual	void SetFlipBackfacingNormals( bool value ) = 0;

		/**
		 * Get the FlipBackfacingNormals flag. If set, then normals will be flipped if 
		 * they are back facing, i.e., pointing into the surface. This may introduce artifacts 
		 * on geometries that have correctly facing normals, so only use for geometries 
		 * with known back-facing normals. 
		 * @return the current value of the FlipBackfacingNormals bool 
		 */
		virtual	bool GetFlipBackfacingNormals(  ) = 0;

		/**
		 * Set the GenerateTangentSpaceNormals flag. If set, then the generated normal 
		 * map will be in tangent space. Please note that the geometry casted to must 
		 * contain tangent space fields. 
		 * @param value is the value to which the GenerateTangentSpaceNormals bool is 
		 * to be set 
		 */
		virtual	void SetGenerateTangentSpaceNormals( bool value ) = 0;

		/**
		 * Get the GenerateTangentSpaceNormals flag. If set, then the generated normal 
		 * map will be in tangent space. Please note that the geometry casted to must 
		 * contain tangent space fields. 
		 * @return the current value of the GenerateTangentSpaceNormals bool 
		 */
		virtual	bool GetGenerateTangentSpaceNormals(  ) = 0;

		/**
		 * Set the FlipGreen flag. If set, the green channel is assumed to point along 
		 * the negative bitangent instead of the positive. 
		 * @param value is the value the FlipGreen bool is to be set 
		 */
		virtual	void SetFlipGreen( bool value ) = 0;

		/**
		 * Get the FlipGreen flag. If set, the green channel is assumed to point along 
		 * the negative bitangent instead of the positive. 
		 * @return the current value of the FlipGreen bool 
		 */
		virtual	bool GetFlipGreen(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an INormalCasterSettings
	 */
	typedef CountedPointer<INormalCasterSettings> spNormalCasterSettings;

	/// Conditional build define for the interface INormalCasterSettings
	#define InterfaceDefined_SimplygonSDK_INormalCasterSettings 1

	class IScene;
	class IGeometryData;

	/**
	 * Repairs normals on a processed geometry. 
	 */
	class INormalRepairer : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if INormalRepairer is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("INormalRepairer",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a INormalRepairer pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a INormalRepairer pointer
		 * @return a pointer to the INormalRepairer object, if the cast can be made, and a NULL pointer otherwise
		 */
		static INormalRepairer *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("INormalRepairer") )
				return static_cast<INormalRepairer*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene 
		 * @param value the scene object to use 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene 
		 * @return the input scene 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Set the ID of the SelectionSet containing the meshes to process If the ID is 
		 * -1, all meshes are selected 
		 * @param value the selection set id to use 
		 */
		virtual	void SetSelectionSetID( rid value ) = 0;

		/**
		 * Set the ID of the SelectionSet containing the meshes to process If the ID is 
		 * -1, all meshes are selected 
		 * @return the ID of the SelectionSet 
		 */
		virtual	rid GetSelectionSetID(  ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If SetName is not found, all 
		 * geometries in the scene will be processed. If both ID and Name are set, ID 
		 * will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetProcessSelectionSetName( const char * value ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to NULL, all geometries 
		 * in the scene will be processed. If both ID and Name are set, ID will be used. 
		 * @return the current SelectionSet name 
		 */
		virtual	rstring GetProcessSelectionSetName(  ) = 0;

		/**
		 * Get whether to recalculate new normals. 
		 * @return the current value of RecalculateNormals 
		 */
		virtual	bool GetRecalculateNormals(  ) = 0;

		/**
		 * Set whether to recalculate new normals. 
		 * @param value the new value of RecalculateNormals 
		 */
		virtual	void SetRecalculateNormals( bool value ) = 0;

		/**
		 * Get the edge angle above which an edge is considered "hard". Note! The angle 
		 * is in radians. 
		 * @return the current value of HardEdgeAngleInRadians 
		 */
		virtual	real GetHardEdgeAngleInRadians(  ) = 0;

		/**
		 * Set the edge angle above which an edge is considered "hard". Note! The angle 
		 * is in radians. 
		 * @param value is the value to which HardEdgeAngleInRadians is to be set 
		 */
		virtual	void SetHardEdgeAngleInRadians( real value ) = 0;

		/**
		 * Get the use borders flag mask. Edges with a matching BorderFlag will be "hard". 
		 *  If for example SGBORDER_TEXCOORD0 is being used, all edges with an UV border 
		 * will be hard shaded. 
		 * @return the current value of the BorderFlagsMask 
		 */
		virtual	unsigned int GetBorderFlagsMask(  ) = 0;

		/**
		 * Set the use borders flag mask. Edges with a matching BorderFlag will be "hard". 
		 *  If for example SGBORDER_TEXCOORD0 is being used, all edges with an UV border 
		 * will be hard shaded. 
		 * @param value is the value to which the BorderFlagsMask is to be set 
		 */
		virtual	void SetBorderFlagsMask( unsigned int value ) = 0;

		/**
		 * Get the ScaleByArea flag. If set, the influence of a triangle normal upon the 
		 * vertex normals is scaled by the area of the triangle. 
		 * @return the current value of the ScaleByArea bool 
		 */
		virtual	bool GetScaleByArea(  ) = 0;

		/**
		 * Set the ScaleByArea flag. If set, the influence of a triangle normal upon the 
		 * vertex normals is scaled by the area of the triangle. 
		 * @param value is the value to which the ScaleByArea bool is to be set 
		 */
		virtual	void SetScaleByArea( bool value ) = 0;

		/**
		 * Get the ScaleByAngle flag. If set, the influence of a triangle normal upon 
		 * the vertex normals is scaled by the angle of the corner at the vertex. 
		 * @return the current value of the ScaleByAngle bool 
		 */
		virtual	bool GetScaleByAngle(  ) = 0;

		/**
		 * Set the ScaleByAngle flag. If set, the influence of a triangle normal upon 
		 * the vertex normals is scaled by the angle of the corner at the vertex. 
		 * @param value is the value to which the ScaleByAngle bool is to be set 
		 */
		virtual	void SetScaleByAngle( bool value ) = 0;

		/**
		 * Get the RepairOnlyInvalidNormals flag If set, then only those normals that 
		 * are invalid (backfacing, zero length etc) will be fixed. 
		 * @return the current value of the RepairOnlyInvalidNormals bool 
		 */
		virtual	bool GetRepairOnlyInvalidNormals(  ) = 0;

		/**
		 * Set the RepairOnlyInvalidNormals flag If set, then only those normals that 
		 * are invalid (backfacing, zero length etc) will be fixed. 
		 * @param value is the value to which the RepairOnlyInvalidNormals bool is to 
		 * be set 
		 */
		virtual	void SetRepairOnlyInvalidNormals( bool value ) = 0;

		/**
		 * Get the SnapNormalsToFlatSurfaces flag. If set, perfectly flat shaded surfaces 
		 * will be created, where possible. Normals adjacent to perfectly geometrically 
		 * flat surfaces will be recalculated to have the same direction as for those 
		 * of the adjacent flat surface. 
		 * @return the current value of the SnapNormalsToFlatSurfaces 
		 */
		virtual	bool GetSnapNormalsToFlatSurfaces(  ) = 0;

		/**
		 * Set the SnapNormalsToFlatSurfaces flag. If set, perfectly flat shaded surfaces 
		 * will be created, where possible. Normals adjacent to perfectly geometrically 
		 * flat surfaces will be recalculated to have the same direction as for those 
		 * of the adjacent flat surface. 
		 * @param value to use for SnapNormalsToFlatSurfaces 
		 */
		virtual	void SetSnapNormalsToFlatSurfaces( bool value ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetSnapNormalsToFlatSurfaces GetSnapNormalsToFlatSurfaces() \endlink 
		 * instead. Use GetSnapNormalsToFlatSurfaces instead. 
		 */
		virtual	SGDEPRECATED bool GetDetectPrimitiveNormals(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link SetSnapNormalsToFlatSurfaces SetSnapNormalsToFlatSurfaces() \endlink 
		 * instead. Use SetSnapNormalsToFlatSurfaces instead. 
		 * @param detectPrimitiveNormals the desired flag value 
		 */
		virtual	SGDEPRECATED void SetDetectPrimitiveNormals( bool detectPrimitiveNormals ) = 0;

		/**
		 * Get setting for whether and how to flip the faces and or normals in the scene. 
		 * SG_FLIPFACESANDNORMALS_NONE - Don't flip SG_FLIPFACESANDNORMALS_AUTOMATIC - 
		 * Automatically detect and fix backfaces per part SG_FLIPFACESANDNORMALS_MAKEDOUBLESIDED 
		 * - Append inverted winded triangles with inverted normals to the geometry SG_FLIPFACESANDNORMALS_INVERTFACES 
		 * - Invert the winding of the triangles SG_FLIPFACESANDNORMALS_INVERTNORMALS 
		 * - Invert the vertex normals SG_FLIPFACESANDNORMALS_INVERTFACESANDNORMALS - 
		 * Invert winding of all triangles and vertex normals 
		 */
		virtual	unsigned int GetFlipFacesAndNormals(  ) = 0;

		/**
		 * Get setting for whether and how to flip the faces and or normals in the scene. 
		 * SG_FLIPFACESANDNORMALS_NONE - Don't flip SG_FLIPFACESANDNORMALS_AUTOMATIC - 
		 * Automatically detect and fix backfaces per part SG_FLIPFACESANDNORMALS_MAKEDOUBLESIDED 
		 * - Append inverted winded triangles with inverted normals to the geometry SG_FLIPFACESANDNORMALS_INVERTFACES 
		 * - Invert the winding of the triangles SG_FLIPFACESANDNORMALS_INVERTNORMALS 
		 * - Invert the vertex normals SG_FLIPFACESANDNORMALS_INVERTFACESANDNORMALS - 
		 * Invert winding of all triangles and vertex normals 
		 * @param value the desired flag value 
		 */
		virtual	void SetFlipFacesAndNormals( unsigned int value ) = 0;

		/**
		 * Get the Geometry object. 
		 * @return the current geometry object 
		 */
		virtual	CountedPointer<IGeometryData> GetGeometry(  ) = 0;

		/**
		 * Set the Geometry object. 
		 * @param value is the geometry that is to be repaired 
		 */
		virtual	void SetGeometry( IGeometryData *value ) = 0;

		/**
		 * Set true to allow for double sided triangles 
		 * @param value the desired value of the flag 
		 */
		virtual	void SetAutoCorrectionAllowsDoubleSided( bool value ) = 0;

		/**
		 * Set the threshold ratio for auto correction 
		 * @param value the desired threshold ratio 
		 */
		virtual	void SetAutoCorrectionRatioThreshold( real value ) = 0;

		/**
		 * Set the threshold ratio for auto correction to treat triangle as single sided 
		 * @param value the desired threshold 
		 */
		virtual	void SetAutoCorrectionSingleSidedThreshold( real value ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an INormalRepairer
	 */
	typedef CountedPointer<INormalRepairer> spNormalRepairer;

	/// Conditional build define for the interface INormalRepairer
	#define InterfaceDefined_SimplygonSDK_INormalRepairer 1

	class IDisplacementCasterSettings;

	/**
	 * IDisplacementCaster is used to store the delta-values between the original and 
	 * processed geometries. The values are divided by a scaling value, and clamped 
	 * into the -1 -> 1 range before being stored into an image. 
	 */
	class IDisplacementCaster : public IMaterialCaster
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IDisplacementCaster is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IDisplacementCaster",type)==0 )
				return true;
			return IMaterialCaster::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IDisplacementCaster pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IDisplacementCaster pointer
		 * @return a pointer to the IDisplacementCaster object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IDisplacementCaster *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IDisplacementCaster") )
				return static_cast<IDisplacementCaster*>(ptr);
			return NULL;
			}

		/**
		 * Set the DistanceScaling value. All the delta values are divided by this value 
		 * before storing them into an image. 
		 * @param DistanceScaling the scale factor 
		 */
		virtual	void SetDistanceScaling( real DistanceScaling ) = 0;

		/**
		 * Get the DistanceScaling value. All the delta values are divided by this value 
		 * before storing them into an image. 
		 * @return The current DistanceScaling value 
		 */
		virtual	real GetDistanceScaling(  ) = 0;

		/**
		 * Set the GenerateScalarDisplacement value. If set to true, the size of the displacement 
		 *  vectors' components in the direction of the interpolated normal are stored 
		 * in the displacement map, instead of the displacement vectors. 
		 * @param GenerateScalarDisplacement the desired value 
		 */
		virtual	void SetGenerateScalarDisplacement( bool GenerateScalarDisplacement ) = 0;

		/**
		 * Get the GenerateScalarDisplacement value. If set to true, the size of the displacement 
		 *  vectors' components in the direction of the interpolated normal are stored 
		 * in the displacement map, instead of the displacement vectors. 
		 * @return The current GenerateScalarDisplacement flag 
		 */
		virtual	bool GetGenerateScalarDisplacement(  ) = 0;

		/**
		 * Set the GenerateTangentSpaceDisplacement value. If set, the displacement vectors 
		 *  are transformed into the destination objects' tangent space 
		 * @param GenerateTangentSpaceDisplacement the desired value 
		 */
		virtual	void SetGenerateTangentSpaceDisplacement( bool GenerateTangentSpaceDisplacement ) = 0;

		/**
		 * Get the GenerateTangentSpaceDisplacement value. If set, the displacement vectors 
		 *  are transformed into the destination objects' tangent space 
		 * @return The current GenerateTangentSpaceDisplacement flag 
		 */
		virtual	bool GetGenerateTangentSpaceDisplacement(  ) = 0;

		/**
		 * Set the texture coordinate level to use for the tangent space displacement 
		 * vectors. 
		 * @param NormalMapTextureLevel the TexCoord level index to use 
		 */
		virtual	void SetNormalMapTextureLevel( rid NormalMapTextureLevel ) = 0;

		/**
		 * Get the texture coordinate level to use for the tangent space displacement 
		 * vectors. 
		 * @return The current NormalMapTextureLevel value 
		 */
		virtual	rid GetNormalMapTextureLevel(  ) = 0;

		/**
		 * Get the displacement caster settings object 
		 * @return Displacement caster settings object 
		 */
		virtual	CountedPointer<IDisplacementCasterSettings> GetDisplacementCasterSettings(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IDisplacementCaster
	 */
	typedef CountedPointer<IDisplacementCaster> spDisplacementCaster;

	/// Conditional build define for the interface IDisplacementCaster
	#define InterfaceDefined_SimplygonSDK_IDisplacementCaster 1


	/**
	 * IDisplacementCasterSettings is used to store the settings for a displacement 
	 * caster 
	 */
	class IDisplacementCasterSettings : public IMaterialCasterSettings
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IDisplacementCasterSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IDisplacementCasterSettings",type)==0 )
				return true;
			return IMaterialCasterSettings::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IDisplacementCasterSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IDisplacementCasterSettings pointer
		 * @return a pointer to the IDisplacementCasterSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IDisplacementCasterSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IDisplacementCasterSettings") )
				return static_cast<IDisplacementCasterSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the DistanceScaling value. All the delta values are divided by this value 
		 * before storing them into an image. 
		 * @param value The desired DistanceScaling value 
		 */
		virtual	void SetDistanceScaling( real value ) = 0;

		/**
		 * Get the DistanceScaling value. All the delta values are divided by this value 
		 * before storing them into an image. 
		 * @return The current DistanceScaling value 
		 */
		virtual	real GetDistanceScaling(  ) = 0;

		/**
		 * Set the GenerateScalarDisplacement value. If set to true, the size of the displacement 
		 *  vectors' components in the direction of the interpolated normal are stored 
		 * in the displacement map, instead of the displacement vectors. 
		 * @param value The desired GenerateScalarDisplacement flag 
		 */
		virtual	void SetGenerateScalarDisplacement( bool value ) = 0;

		/**
		 * Get the GenerateScalarDisplacement value. If set to true, the size of the displacement 
		 *  vectors' components in the direction of the interpolated normal are stored 
		 * in the displacement map, instead of the displacement vectors. 
		 * @return The current GenerateScalarDisplacement flag 
		 */
		virtual	bool GetGenerateScalarDisplacement(  ) = 0;

		/**
		 * Set the GenerateTangentSpaceDisplacement value. If set, the displacement vectors 
		 *  are transformed into the destination objects' tangent space 
		 * @param value The desired GenerateTangentSpaceDisplacement flag 
		 */
		virtual	void SetGenerateTangentSpaceDisplacement( bool value ) = 0;

		/**
		 * Get the GenerateTangentSpaceDisplacement value. If set, the displacement vectors 
		 *  are transformed into the destination objects' tangent space 
		 * @return The current GenerateTangentSpaceDisplacement flag 
		 */
		virtual	bool GetGenerateTangentSpaceDisplacement(  ) = 0;

		/**
		 * Set the texture coordinate level to use for the tangent space displacement 
		 * vectors. 
		 * @param value The desired NormalMapTextureLevel value 
		 */
		virtual	void SetNormalMapTextureLevel( rid value ) = 0;

		/**
		 * Get the texture coordinate level to use for the tangent space displacement 
		 * vectors. 
		 * @return The current NormalMapTextureLevel value 
		 */
		virtual	rid GetNormalMapTextureLevel(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IDisplacementCasterSettings
	 */
	typedef CountedPointer<IDisplacementCasterSettings> spDisplacementCasterSettings;

	/// Conditional build define for the interface IDisplacementCasterSettings
	#define InterfaceDefined_SimplygonSDK_IDisplacementCasterSettings 1

	class IGeometryData;
	class ISceneNode;

	/**
	 * IGeometryTangentCalculator calculates tangent vectors for geometry data objects. 
	 *  One corner field of texture coordinates must exist, as well as normals. The 
	 * Tangents are placed into two corner fields called "Tangents" and "Bitangents". 
	 * If any of these fields already exists, it is replaced by the new field. 
	 */
	class ITangentCalculator : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ITangentCalculator is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ITangentCalculator",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ITangentCalculator pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ITangentCalculator pointer
		 * @return a pointer to the ITangentCalculator object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ITangentCalculator *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ITangentCalculator") )
				return static_cast<ITangentCalculator*>(ptr);
			return NULL;
			}

		/**
		 * Calculates the tangents of a geometry data object. 
		 * @param geom the geometry object receives new fields, as described in the class 
		 * description. 
		 */
		virtual	void CalculateTangents( IGeometryData *geom ) = 0;

		/**
		 * Calculates the tangents of a tree of scene nodes (only the geometry nodes of 
		 * course). 
		 * @param node is the scene node that will have itselfs and its childrens tangents 
		 * calculated 
		 */
		virtual	void CalculateTangents( ISceneNode *node ) = 0;

		/**
		 * The TexCoords field id to use for the tangent calculation. If set to -1, all 
		 * texture coordinates will get corresponding tangent fields. 
		 * @param value the TexCoords set id 
		 */
		virtual	void SetTexCoordsSetId( rid value ) = 0;

		/**
		 * Sets the tangent calculator type, types listed in SimplygonSDK::TangentSpaceMethod 
		 * @param type the desired tangent calculator type Value cannot be less than SG_TANGENTSPACEMETHOD_ORTHONORMAL. 
		 * Value cannot be greater than SG_TANGENTSPACEMETHOD_ORTHONORMAL_LEFTHANDED. 
		 *  
		 */
		virtual	void SetTangentCalculatorType( rid type ) = 0;

		/**
		 * Gets the tangent calculator type, types listed in SimplygonSDK::TangentSpaceMethod 
		 * @return the current tangent calculator type 
		 */
		virtual	rid GetTangentCalculatorType(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ITangentCalculator
	 */
	typedef CountedPointer<ITangentCalculator> spTangentCalculator;

	/// Conditional build define for the interface ITangentCalculator
	#define InterfaceDefined_SimplygonSDK_ITangentCalculator 1


	/**
	 * Settings for handling unconnected parts and unwanted gaps in the geometry. It 
	 * consists of two parts, Welding and T-junction removal. Welding merges vertices 
	 * that are closer than a set distance from each other. T-junction removal finds 
	 * open edges that lies within a set distance from other open edges, but without 
	 * matching vertices. It then splits those triangles into smaller, matching triangles 
	 * and fuses them together. 
	 */
	class IRepairSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IRepairSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IRepairSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IRepairSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IRepairSettings pointer
		 * @return a pointer to the IRepairSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IRepairSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IRepairSettings") )
				return static_cast<IRepairSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the T-Junction distance, below which, the T-Junctions will be welded 
		 * @param value is the value to which TjuncDist will be set Value cannot be less 
		 * than 0. 
		 */
		virtual	void SetTjuncDist( real value ) = 0;

		/**
		 * Get the T-Junction distance, below which, the T-Junctions will be welded 
		 * @return the current value of TjuncDist 
		 */
		virtual	real GetTjuncDist(  ) = 0;

		/**
		 * Set the UseTJunctionRemover flag, if set, t-junctions will be removed. If enabled, 
		 *  it also requires UseWelding to be on. 
		 * @param value is the bool to which UseTJunctionRemover will be set 
		 */
		virtual	void SetUseTJunctionRemover( bool value ) = 0;

		/**
		 * Get the UseTJunctionRemover flag, if set, t-junctions will be removed. If enabled, 
		 *  it also requires UseWelding to be on. 
		 * @return the current value of UseTJunctionRemover 
		 */
		virtual	bool GetUseTJunctionRemover(  ) = 0;

		/**
		 * Set the Welding distance, below which, the vertices will be welded 
		 * @param value is the value to which WeldDist will be set Value cannot be less 
		 * than 0. 
		 */
		virtual	void SetWeldDist( real value ) = 0;

		/**
		 * Get the Welding distance, below which, the vertices will be welded 
		 * @return the current value of WeldDist 
		 */
		virtual	real GetWeldDist(  ) = 0;

		/**
		 * Set the WeldOnlyBorderVertices flag, if set, only vertices that are on the 
		 * border (as reported by the VertexBorder boolean field) are considered for the 
		 * welding 
		 * @param value is the bool to which WeldOnlyBorderVertices will be set 
		 */
		virtual	void SetWeldOnlyBorderVertices( bool value ) = 0;

		/**
		 * Get the WeldOnlyBorderVertices flag, if set, only vertices that are on the 
		 * border (as reported by the VertexBorder boolean field) are considered for the 
		 * welding 
		 * @return the current value of WeldOnlyBorderVertices 
		 */
		virtual	bool GetWeldOnlyBorderVertices(  ) = 0;

		/**
		 * Set the WeldOnlyWithinMaterial flag, If set, only vertices that are within 
		 * the same material are welded. Vertices on material borders are locked. 
		 * @param value is the bool to which WeldOnlyWithinMaterial will be set 
		 */
		virtual	void SetWeldOnlyWithinMaterial( bool value ) = 0;

		/**
		 * Get the WeldOnlyWithinMaterial flag. If set, only vertices that are within 
		 * the same material are welded. Vertices on material borders are locked. 
		 * @return the current value of WeldOnlyWithinMaterial 
		 */
		virtual	bool GetWeldOnlyWithinMaterial(  ) = 0;

		/**
		 * Set the WeldOnlyWithinSceneNode flag, If set, only vertices that are within 
		 * the same scene node are welded. 
		 * @param value is the bool to which WeldOnlyWithinSceneNode will be set 
		 */
		virtual	void SetWeldOnlyWithinSceneNode( bool value ) = 0;

		/**
		 * Get the WeldOnlyWithinSceneNode flag. If set, only vertices that are within 
		 * the same scene node are welded. 
		 * @return the current value of WeldOnlyWithinSceneNode 
		 */
		virtual	bool GetWeldOnlyWithinSceneNode(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetWeldOnlyBetweenSceneNodes GetWeldOnlyBetweenSceneNodes() \endlink 
		 * instead. Use GetWeldOnlyBetweenSceneNodes instead 
		 */
		virtual	SGDEPRECATED bool GetWeldOnlyObjectBoundary(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link SetWeldOnlyBetweenSceneNodes SetWeldOnlyBetweenSceneNodes() \endlink 
		 * instead. Use SetWeldOnlyBetweenSceneNodes instead 
		 * @param weldOnlyObjectBoundary is the bool to which weldOnlyObjectBoundary will 
		 * be set 
		 */
		virtual	SGDEPRECATED void SetWeldOnlyObjectBoundary( bool weldOnlyObjectBoundary ) = 0;

		/**
		 * Set the WeldOnlyBetweenSceneNodes flag. If set, only vertices that belong to 
		 * different scene nodes are considered for the welding. Affects both welder and 
		 * t-junction remover. 
		 * @param value is the bool to which WeldOnlyBetweenSceneNode will be set 
		 */
		virtual	void SetWeldOnlyBetweenSceneNodes( bool value ) = 0;

		/**
		 * Get the WeldOnlyBetweenSceneNodes flag. If set, only vertices that belong to 
		 * different scene nodes are considered for the welding. Affects both welder and 
		 * t-junction remover. 
		 * @return the current value of WeldOnlyBetweenSceneNode 
		 */
		virtual	bool GetWeldOnlyBetweenSceneNodes(  ) = 0;

		/**
		 * Set the UseWelding flag. If set, vertices within each others welding distance 
		 * will be welded together. 
		 * @param value is the bool to which UseWelding will be set 
		 */
		virtual	void SetUseWelding( bool value ) = 0;

		/**
		 * Get the UseWelding flag. If set, vertices within each others welding distance 
		 * will be welded together. 
		 * @return the current value of UseWelding 
		 */
		virtual	bool GetUseWelding(  ) = 0;

		/**
		 * Set the number of ProgressivePasses. Minimum is 1, but higher numbers give 
		 * a better quality, at the expense of longer running time. 
		 * @param value is the integer to which ProgressivePasses will be set Value cannot 
		 * be less than 1. 
		 */
		virtual	void SetProgressivePasses( unsigned int value ) = 0;

		/**
		 * Get the number of ProgressivePasses. Minimum is 1, but higher numbers give 
		 * a better quality, at the expense of longer running time. 
		 * @return the current value of ProgressivePasses 
		 */
		virtual	unsigned int GetProgressivePasses(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IRepairSettings
	 */
	typedef CountedPointer<IRepairSettings> spRepairSettings;

	/// Conditional build define for the interface IRepairSettings
	#define InterfaceDefined_SimplygonSDK_IRepairSettings 1


	/**
	 * Settings for the Simplygon Reduction/Remeshing classes With the BoneSettings 
	 * object, the user can setup all the parameters for how the BoneLOD processor 
	 * should simplify the links between geometries and bones. Bones can be un-linked 
	 * from the geometry either with a set ratio or depending on a selected pixel size 
	 * on screen. 
	 */
	class IBoneSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IBoneSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IBoneSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IBoneSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IBoneSettings pointer
		 * @return a pointer to the IBoneSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IBoneSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IBoneSettings") )
				return static_cast<IBoneSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the flag mask that decides what types of reduction targets will be used 
		 * <ul> <li>SG_BONEREDUCTIONTARGET_BONECOUNT -> the reducer will stop when the 
		 * set bone-count has been reached.</li> <li>SG_BONEREDUCTIONTARGET_BONERATIO 
		 * -> the reducer will stop when the set bone-ratio has been reached.</li> <li>SG_BONEREDUCTIONTARGET_MAXDEVIATION 
		 *  -> the reducer will stop when the set max deviation has been reached.</li> 
		 * <li>SG_BONEREDUCTIONTARGET_ONSCREENSIZE -> the reducer will stop when the asset 
		 * will visually pass for the original at the set pixel size.</li> </ul> 
		 * @param value is the desired value of BoneReductionTargets 
		 */
		virtual	void SetBoneReductionTargets( unsigned int value ) = 0;

		/**
		 * Get the flag mask that decides what types of reduction targets will be used 
		 * <ul> <li>SG_BONEREDUCTIONTARGET_BONECOUNT -> the reducer will stop when the 
		 * set bone-count has been reached.</li> <li>SG_BONEREDUCTIONTARGET_BONERATIO 
		 * -> the reducer will stop when the set bone-ratio has been reached.</li> <li>SG_BONEREDUCTIONTARGET_MAXDEVIATION 
		 *  -> the reducer will stop when the set max deviation has been reached.</li> 
		 * <li>SG_BONEREDUCTIONTARGET_ONSCREENSIZE -> the reducer will stop when the asset 
		 * will visually pass for the original at the set pixel size.</li> </ul> 
		 * @return the current value of ReductionTargets. 
		 */
		virtual	unsigned int GetBoneReductionTargets(  ) = 0;

		/**
		 * Set the stop condition for the reducer. <ul> <li>SG_STOPCONDITION_ANY -> the 
		 * reducer will stop when any single one of the set reduction targets have been 
		 * reached</li> <li>SG_STOPCONDITION_ALL -> the reducer will stop when all enabled 
		 * reduction targets have been set.</li> </ul> 
		 * @param value is the flag to which StopCondition will be set. 
		 */
		virtual	void SetStopCondition( unsigned int value ) = 0;

		/**
		 * Get the stop condition for the reducer. <ul> <li>SG_STOPCONDITION_ANY -> the 
		 * reducer will stop when any single one of the set reduction targets have been 
		 * reached</li> <li>SG_STOPCONDITION_ALL -> the reducer will stop when all enabled 
		 * reduction targets have been set.</li> </ul> 
		 */
		virtual	unsigned int GetStopCondition(  ) = 0;

		/**
		 * Set the wanted reduction ratio. The range is 0->1. 
		 * @param value is the value to which ReductionRatio will be set 
		 */
		virtual	void SetBoneRatio( real value ) = 0;

		/**
		 * Get the wanted reduction ratio. The range is 0->1. 
		 * @return the current value of ReductionRatio 
		 */
		virtual	real GetBoneRatio(  ) = 0;

		/**
		 * Set the wanted triangle count. The range is 0->inf. 
		 * @param value is the value to which ReductionRatio will be set 
		 */
		virtual	void SetBoneCount( unsigned int value ) = 0;

		/**
		 * Get the wanted reduction ratio. The range is 0->inf. 
		 * @return the current value of TriangleCount 
		 */
		virtual	unsigned int GetBoneCount(  ) = 0;

		/**
		 * Set the MaxDeviation value, the maximum surface-deviation between the reduced 
		 * geometry and the original. 
		 * @param value is the value to which MaxDeviation will be set 
		 */
		virtual	void SetMaxDeviation( real value ) = 0;

		/**
		 * Get the MaxDeviation value, the maximum surface-deviation between the reduced 
		 * geometry and the original. 
		 * @return the current value of MaxDeviation 
		 */
		virtual	real GetMaxDeviation(  ) = 0;

		/**
		 * Set the OnScreenSize value, the maximum surface-deviation between the reduced 
		 * geometry and the original. 
		 * @param value is the value to which OnScreenSize will be set 
		 */
		virtual	void SetOnScreenSize( unsigned int value ) = 0;

		/**
		 * Get the OnScreenSize value, the maximum surface-deviation between the reduced 
		 * geometry and the original. 
		 * @return the current value of OnScreenSize 
		 */
		virtual	unsigned int GetOnScreenSize(  ) = 0;

		/**
		 * Decides the maximum limit of how many bones can be connected to each vertex. 
		 * If for example set to 1, then each vertex will only be linked to 1 bone. 
		 * @param value the desired maximum bones per vertex 
		 */
		virtual	void SetMaxBonePerVertex( unsigned int value ) = 0;

		/**
		 * Decides the maximum limit of how many bones can be connected to each vertex. 
		 * If for example set to 1, then each vertex will only be linked to 1 bone. 
		 * @return value the maximum bones per vertex 
		 */
		virtual	unsigned int GetMaxBonePerVertex(  ) = 0;

		/**
		 * Decides whether unused bones should be removed. 
		 * @param value the desired flag value 
		 */
		virtual	void SetRemoveUnusedBones( bool value ) = 0;

		/**
		 * Decides whether unused bones should be removed. 
		 * @return value the current flag value 
		 */
		virtual	bool GetRemoveUnusedBones(  ) = 0;

		/**
		 * Decides whether unused bones should be removed. 
		 * @param value the desired flag value 
		 */
		virtual	void SetLimitBonesPerVertex( bool value ) = 0;

		/**
		 * Decides whether unused bones should be removed. 
		 * @return value the current flag value 
		 */
		virtual	bool GetLimitBonesPerVertex(  ) = 0;

		/**
		 * Enables or disables the bone reduction processing that actually removes bones. 
		 * Even with this flag off, the bones per vertex can still be limited. 
		 * @param value the desired flag value 
		 */
		virtual	void SetUseBoneReducer( bool value ) = 0;

		/**
		 * Enables or disables the bone reduction processing that actually removes bones. 
		 * Even with this flag off, the bones per vertex can still be limited. 
		 * @return value the current flag value 
		 */
		virtual	bool GetUseBoneReducer(  ) = 0;

		/**
		 * The ID of the selection set that contains all of the bones that will be locked, 
		 *  and cannot be removed in the BoneLOD process. If a bone is locked, all its 
		 * ancestors will be locked also. If the ID is -1, then no bones are selected 
		 * @param value the ID of the selection set 
		 */
		virtual	void SetLockBoneSelectionSetID( rid value ) = 0;

		/**
		 * The ID of the selection set that contains all of the bones that will be locked, 
		 *  and cannot be removed in the BoneLOD process. If a bone is locked, all its 
		 * ancestors will be locked also. If the ID is -1, then no bones are selected 
		 * @return the ID of the selection set 
		 */
		virtual	rid GetLockBoneSelectionSetID(  ) = 0;

		/**
		 * The name of the selection set that contains all of the bones that will be locked, 
		 *  and cannot be removed in the BoneLOD process. If a bone is locked, all its 
		 * ancestors will be locked also. If the name is NULL, then no bones are selected 
		 * If both ID and Name are set, ID will be used. 
		 * @param value the name of the selection set 
		 */
		virtual	void SetLockBoneSelectionSetName( const char * value ) = 0;

		/**
		 * The name of the selection set that contains all of the bones that will be locked, 
		 *  and cannot be removed in the BoneLOD process. If a bone is locked, all its 
		 * ancestors will be locked also. If the name is NULL, then no bones are selected 
		 * If both ID and Name are set, ID will be used. 
		 */
		virtual	rstring GetLockBoneSelectionSetName(  ) = 0;

		/**
		 * The ID of the selection set that contains all of the bones that will be forced 
		 * to be removed in the BoneLOD process, UNLESS they are locked. If a bone is 
		 * set to be removed, all its descendants will be removed also. If the ID is -1, 
		 * then no bones are selected 
		 * @param value the ID of the selection set 
		 */
		virtual	void SetRemoveBoneSelectionSetID( rid value ) = 0;

		/**
		 * The ID of the selection set that contains all of the bones that will be forced 
		 * to be removed in the BoneLOD process, UNLESS they are locked. If a bone is 
		 * set to be removed, all its descendants will be removed also. If the ID is -1, 
		 * then no bones are selected 
		 * @return the ID of the selection set 
		 */
		virtual	rid GetRemoveBoneSelectionSetID(  ) = 0;

		/**
		 * The name of the selection set that contains all of the bones that will be forced 
		 *  to be removed in the BoneLOD process, UNLESS they are locked. If a bone is 
		 * set to be removed, all its descendants will be removed also. If the name is 
		 * NULL, then no bones are selected If both ID and Name are set, ID will be used. 
		 * @param value the name of the selection set 
		 */
		virtual	void SetRemoveBoneSelectionSetName( const char * value ) = 0;

		/**
		 * The name of the selection set that contains all of the bones that will be forced 
		 *  to be removed in the BoneLOD process, UNLESS they are locked. If a bone is 
		 * set to be removed, all its descendants will be removed also. If the name is 
		 * NULL, then no bones are selected If both ID and Name are set, ID will be used. 
		 *  
		 */
		virtual	rstring GetRemoveBoneSelectionSetName(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IBoneSettings
	 */
	typedef CountedPointer<IBoneSettings> spBoneSettings;

	/// Conditional build define for the interface IBoneSettings
	#define InterfaceDefined_SimplygonSDK_IBoneSettings 1


	/**
	 * Settings for the IReductionProcessor class. Handles all the settings for Simplygon’s 
	 *  simplification algorithms. The user can tell the reduction processor which 
	 * features are important by using the “FeatureFlags” parameter, and also the relative 
	 * importance of these features using the Set{feature}Importance methods. Also 
	 * settings for things like symmetry are set here. 
	 */
	class IReductionSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IReductionSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IReductionSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IReductionSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IReductionSettings pointer
		 * @return a pointer to the IReductionSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IReductionSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IReductionSettings") )
				return static_cast<IReductionSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the flag mask that decides what types of reduction targets will be used 
		 * <ul> <li>SG_REDUCTIONTARGET_TRIANGLECOUNT -> the reducer will stop when the 
		 * set triangle-count has been reached.</li> <li>SG_REDUCTIONTARGET_TRIANGLERATIO 
		 * -> the reducer will stop when the set triangle-ratio has been reached.</li> 
		 * <li>SG_REDUCTIONTARGET_MAXDEVIATION -> the reducer will stop when the set max 
		 * deviation has been reached.</li> <li>SG_REDUCTIONTARGET_ONSCREENSIZE -> the 
		 * reducer will stop when the asset will visually pass for the original at the 
		 *  set pixel size.</li> </ul> 
		 * @param value is the flag mask to which ReductionTargets will be set. Value 
		 * cannot be greater than SG_REDUCTIONTARGET_ALL. 
		 */
		virtual	void SetReductionTargets( unsigned int value ) = 0;

		/**
		 * Get the flag mask that decides what types of reduction targets will be used 
		 * <ul> <li>SG_REDUCTIONTARGET_TRIANGLECOUNT -> the reducer will stop when the 
		 * set triangle-count has been reached.</li> <li>SG_REDUCTIONTARGET_TRIANGLERATIO 
		 * -> the reducer will stop when the set triangle-ratio has been reached.</li> 
		 * <li>SG_REDUCTIONTARGET_MAXDEVIATION -> the reducer will stop when the set max 
		 * deviation has been reached.</li> <li>SG_REDUCTIONTARGET_ONSCREENSIZE -> the 
		 * reducer will stop when the asset will visually pass for the original at the 
		 *  set pixel size.</li> </ul> 
		 * @return the current value of ReductionTargets. 
		 */
		virtual	unsigned int GetReductionTargets(  ) = 0;

		/**
		 * Set the stop condition for the reducer. <ul> <li>SG_STOPCONDITION_ANY -> the 
		 * reducer will stop when any single one of the set reduction targets have been 
		 * reached</li> <li>SG_STOPCONDITION_ALL -> the reducer will stop when all enabled 
		 * reduction targets have been set.</li> </ul> 
		 * @param value is the flag to which StopCondition will be set. Value cannot be 
		 * less than SG_STOPCONDITION_ANY. Value cannot be greater than SG_STOPCONDITION_ALL. 
		 *  
		 */
		virtual	void SetStopCondition( unsigned int value ) = 0;

		/**
		 * Get the stop condition for the reducer. <ul> <li>SG_STOPCONDITION_ANY -> the 
		 * reducer will stop when any single one of the set reduction targets have been 
		 * reached</li> <li>SG_STOPCONDITION_ALL -> the reducer will stop when all enabled 
		 * reduction targets have been set.</li> </ul> 
		 */
		virtual	unsigned int GetStopCondition(  ) = 0;

		/**
		 * Set the flag that decides how precise the reducer will be during the mesh reduction. 
		 *  <ul> <li>SG_REDUCTIONHEURISTICS_FAST -> Reducer will be faster, but onscreensize 
		 *  and trianglecount will not match perfectly. </li> <li>SG_REDUCTIONHEURISTICS_CONSISTENT 
		 *  -> Reducer will be slower, but onscreensize and trianglecount will consistently 
		 *  give matching results.</li> </ul> 
		 * @param value is the ReductionHeuristics to use. Value cannot be less than SG_REDUCTIONHEURISTICS_FAST. 
		 * Value cannot be greater than SG_REDUCTIONHEURISTICS_CONSISTENT. 
		 * @return the current value of ReductionHeuristics. 
		 */
		virtual	void SetReductionHeuristics( unsigned int value ) = 0;

		/**
		 * Get the flag that decides how precise the reducer will be during the mesh reduction. 
		 *  <ul> <li>SG_REDUCTIONHEURISTICS_FAST -> Reducer will be faster, but onscreensize 
		 *  and trianglecount will not match perfectly. </li> <li>SG_REDUCTIONHEURISTICS_CONSISTENT 
		 *  -> Reducer will be slower, but onscreensize and trianglecount will consistently 
		 *  give matching results.</li> </ul> 
		 * @return the current value of ReductionHeuristics. 
		 */
		virtual	unsigned int GetReductionHeuristics(  ) = 0;

		/**
		 * Set the wanted reduction ratio. The range is 0->1. 
		 * @param value is the value to which ReductionRatio will be set Value cannot 
		 * be less than 0. Value cannot be greater than 1. 
		 */
		virtual	void SetTriangleRatio( real value ) = 0;

		/**
		 * Get the wanted reduction ratio. The range is 0->1. 
		 * @return the current value of ReductionRatio. Default value is 1 
		 */
		virtual	real GetTriangleRatio(  ) = 0;

		/**
		 * Set the wanted triangle count. The range is 0->inf. 
		 * @param value is the value to which TriangleCount will be set Value cannot be 
		 * less than 0. 
		 */
		virtual	void SetTriangleCount( unsigned int value ) = 0;

		/**
		 * Get the wanted reduction ratio. The range is 0->inf. 
		 * @return the current value of TriangleCount 
		 */
		virtual	unsigned int GetTriangleCount(  ) = 0;

		/**
		 * Set the MaxDeviation value, the maximum surface-deviation between the reduced 
		 * geometry and the original. 
		 * @param value is the value to which MaxDeviation will be set Value cannot be 
		 * less than 0. 
		 */
		virtual	void SetMaxDeviation( real value ) = 0;

		/**
		 * Get the MaxDeviation value, the maximum surface-deviation between the reduced 
		 * geometry and the original. 
		 * @return the current value of MaxDeviation 
		 */
		virtual	real GetMaxDeviation(  ) = 0;

		/**
		 * Set the OnScreenSize value, used to calculate the maximum surface-deviation 
		 * allowed, by assuming errors less than 1 pixel onscreen will not be noticable 
		 * @param value is the value to which OnScreenSize will be set Value cannot be 
		 * less than 1. Value cannot be greater than 1000000. 
		 */
		virtual	void SetOnScreenSize( unsigned int value ) = 0;

		/**
		 * Get the OnScreenSize value, used to calculate the maximum surface-deviation 
		 * allowed, by assuming errors less than 1 pixel onscreen will not be noticable 
		 * @return the current value of OnScreenSize 
		 */
		virtual	unsigned int GetOnScreenSize(  ) = 0;

		/**
		 * Set the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @param value is the value to which the importance will be set Value cannot 
		 * be less than 0. Value cannot be greater than SG_MAXIMUM_IMPORTANCE_VALUE. 
		 */
		virtual	void SetGeometryImportance( real value ) = 0;

		/**
		 * Set the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @param value is the value to which the importance will be set Value cannot 
		 * be less than 0. Value cannot be greater than SG_MAXIMUM_IMPORTANCE_VALUE. 
		 */
		virtual	void SetMaterialImportance( real value ) = 0;

		/**
		 * Set the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @param value is the value to which the importance will be set Value cannot 
		 * be less than 0. Value cannot be greater than SG_MAXIMUM_IMPORTANCE_VALUE. 
		 */
		virtual	void SetTextureImportance( real value ) = 0;

		/**
		 * Set the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @param value is the value to which the importance will be set Value cannot 
		 * be less than 0. Value cannot be greater than SG_MAXIMUM_IMPORTANCE_VALUE. 
		 */
		virtual	void SetShadingImportance( real value ) = 0;

		/**
		 * Set the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @param value is the value to which the importance will be set Value cannot 
		 * be less than 0. Value cannot be greater than SG_MAXIMUM_IMPORTANCE_VALUE. 
		 */
		virtual	void SetGroupImportance( real value ) = 0;

		/**
		 * Set the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @param value is the value to which the importance will be set Value cannot 
		 * be less than 0. Value cannot be greater than SG_MAXIMUM_IMPORTANCE_VALUE. 
		 */
		virtual	void SetVertexColorImportance( real value ) = 0;

		/**
		 * Set the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @param value is the value to which the importance will be set Value cannot 
		 * be less than 0. Value cannot be greater than SG_MAXIMUM_IMPORTANCE_VALUE. 
		 */
		virtual	void SetEdgeSetImportance( real value ) = 0;

		/**
		 * Set the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @param value is the value to which the importance will be set Value cannot 
		 * be less than 0. Value cannot be greater than SG_MAXIMUM_IMPORTANCE_VALUE. 
		 */
		virtual	void SetSkinningImportance( real value ) = 0;

		/**
		 * Set the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @param value is the value to which the importance will be set Value cannot 
		 * be less than 0. Value cannot be greater than SG_MAXIMUM_IMPORTANCE_VALUE. 
		 */
		virtual	void SetCurvatureImportance( real value ) = 0;

		/**
		 * Get the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @return the current value of the importance of the feature 
		 */
		virtual	real GetGeometryImportance(  ) = 0;

		/**
		 * Get the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @return the current value of the importance of the feature 
		 */
		virtual	real GetMaterialImportance(  ) = 0;

		/**
		 * Get the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @return the current value of the importance of the feature 
		 */
		virtual	real GetTextureImportance(  ) = 0;

		/**
		 * Get the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @return the current value of the importance of the feature 
		 */
		virtual	real GetShadingImportance(  ) = 0;

		/**
		 * Get the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @return the current value of the importance of the feature 
		 */
		virtual	real GetGroupImportance(  ) = 0;

		/**
		 * Get the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @return the current value of the importance of the feature 
		 */
		virtual	real GetVertexColorImportance(  ) = 0;

		/**
		 * Get the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @return the current value of the importance of the feature 
		 */
		virtual	real GetEdgeSetImportance(  ) = 0;

		/**
		 * Get the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @return the current value of the importance of the feature 
		 */
		virtual	real GetSkinningImportance(  ) = 0;

		/**
		 * Get the importance value of the relevant feature, meaning the relative importance 
		 *  of the specific mesh feature (or disabling an importance entirely by setting 
		 * it to 0.0). The default importance value is 1. A higher number means higher 
		 * importance. 
		 * @return the current value of the importance of the feature 
		 */
		virtual	real GetCurvatureImportance(  ) = 0;

		/**
		 * Set the UseVertexWeighting value. If set, the reducer will weight the reduction 
		 *  based on the weights in the "VertexWeighting" vertex field. 
		 * @param value is the bool to which UseVertexWeights will be set 
		 */
		virtual	void SetUseVertexWeights( bool value ) = 0;

		/**
		 * Get the UseVertexWeighting value. If set, the reducer will weight the reduction 
		 *  based on the weights in the "VertexWeighting" vertex field. 
		 * @return the current value of the UseVertexWeights bool 
		 */
		virtual	bool GetUseVertexWeights(  ) = 0;

		/**
		 * Set the CreateGeomorphGeometry value. If set, the reducer will create a GeometryData 
		 *  object that is a morphed version of the original GeometryData, that can be 
		 * used to morph between the original geometry and the reduced geometry. Should 
		 * not be used together with TJunctionRemover, NormalRecalculation or Material 
		 * LOD. 
		 * @param value is the bool to which CreateGeomorphGeometry will be set 
		 */
		virtual	void SetCreateGeomorphGeometry( bool value ) = 0;

		/**
		 * Get the CreateGeomorphGeometry value. If set, the reducer will create a GeometryData 
		 *  object that is a morphed version of the original GeometryData, that can be 
		 * used to morph between the original geometry and the reduced geometry. Should 
		 * not be used together with TJunctionRemover, NormalRecalculation or Material 
		 * LOD. 
		 * @return the current value of the CreateGeomorphGeometry bool 
		 */
		virtual	bool GetCreateGeomorphGeometry(  ) = 0;

		/**
		 * Set the AllowDegenerateTexCoords flag. If true, texture coordinates are allowed 
		 *  to become degenerate (triangles can have 0 area in texture space). This may 
		 * sometimes look better, but if tangent space normals are used, tangent spaces 
		 * may become degenerate. 
		 * @param value is the bool to which AllowDegenerateTexCoords will be set 
		 */
		virtual	void SetAllowDegenerateTexCoords( bool value ) = 0;

		/**
		 * Get the AllowDegenerateTexCoords flag. If true, texture coordinates are allowed 
		 *  to become degenerate (triangles can have 0 area in texture space). This may 
		 * sometimes look better, but if tangent space normals are used, tangent spaces 
		 * may become degenerate. 
		 * @return the current value of the AllowDegenerateTexCoords bool 
		 */
		virtual	bool GetAllowDegenerateTexCoords(  ) = 0;

		/**
		 * Set the KeepSymmetry flag. If set, the reducer will detect symmetric features, 
		 * and retain symmetry during processing. Please note that for a geometry to be 
		 * considered symmetrical, also the texture coordinates must be mirrored. 
		 * @param value is the bool to which KeepSymmetry will be set 
		 */
		virtual	void SetKeepSymmetry( bool value ) = 0;

		/**
		 * Get the KeepSymmetry flag. If set, the reducer will detect symmetric features, 
		 * and retain symmetry during processing. Please note that for a geometry to be 
		 * considered symmetrical, also the texture coordinates must be mirrored. 
		 * @return the current value of the KeepSymmetry bool 
		 */
		virtual	bool GetKeepSymmetry(  ) = 0;

		/**
		 * Set the AutomaticSymmetryDetection flag. If set, and KeepSymmetry is on, the 
		 * reducer will detect any X,Y,Z symmetry plane and will override any user set 
		 * symmetry plane with the detected one. If no symmetry is found, KeepSymmetry 
		 * will be turned off before reduction. 
		 * @param value is the bool to which AutomaticSymmetryDetection will be set 
		 */
		virtual	void SetUseAutomaticSymmetryDetection( bool value ) = 0;

		/**
		 * Get the AutomaticSymmetryDetection flag. If set, and KeepSymmetry is on, the 
		 * reducer will automatically detect and use any X,Y,Z symmetry plane and will 
		 * override any user set symmetry plane with the detected one. If no symmetry 
		 * is found, KeepSymmetry will be turned off before reduction. 
		 * @return the current value of the AutomaticSymmetryDetection bool 
		 */
		virtual	bool GetUseAutomaticSymmetryDetection(  ) = 0;

		/**
		 * Set the UseSymmetryQuadRetriangulator flag. If true, and KeepSymmetry is on, 
		 * any triangles that have been triangulated from symmetrical quads into unsymmetrical 
		 *  triangles are detected and fixed. The edges of said triangles are flipped 
		 * across the old quad so that the mesh becomes symmetrical again. 
		 * @param value is the bool to which UseSymmetryQuadRetriangulator will be set 
		 *  
		 */
		virtual	void SetUseSymmetryQuadRetriangulator( bool value ) = 0;

		/**
		 * Get the UseSymmetryQuadRetriangulator flag. If true, and KeepSymmetry is on, 
		 * any triangles that have been triangulated from symmetrical quads into unsymmetrical 
		 *  triangles are detected and fixed. The edges of said triangles are flipped 
		 * across the old quad so that the mesh becomes symmetrical again. 
		 * @return the current value of the UseSymmetryQuadRetriangulator bool 
		 */
		virtual	bool GetUseSymmetryQuadRetriangulator(  ) = 0;

		/**
		 * Set what axis to be used for symmetry calculations. 
		 * @param value is the value to which SymmetryAxis will be set. <ul><li>0 = x</li><li>1 
		 * = y</li><li>2 = z</li></ul> Value cannot be less than 0. Value cannot be greater 
		 * than 2. 
		 */
		virtual	void SetSymmetryAxis( unsigned int value ) = 0;

		/**
		 * Get what axis to be used for symmetry calculations. 
		 * @return the current value of SymmetryAxis. <ul><li>0 = x</li><li>1 = y</li><li>2 
		 * = z</li></ul> 
		 */
		virtual	unsigned int GetSymmetryAxis(  ) = 0;

		/**
		 * Set SymmetryOffset, the position on the symmetry axis where the symmetry plane 
		 * is placed. 
		 * @param value is the value to which SymmetryOffset will be set 
		 */
		virtual	void SetSymmetryOffset( real value ) = 0;

		/**
		 * Get SymmetryOffset, the position on the symmetry axis where the symmetry plane 
		 * is placed. 
		 * @return the current value of SymmetryOffset 
		 */
		virtual	real GetSymmetryOffset(  ) = 0;

		/**
		 * Set the SymmetryDetectionTolerance, the tolerance used when detecting symmetry. 
		 *  Values in the ranges 1e-5 to 1e-3 will usually produce good results. This 
		 * specific tolerance corresponds to the off-plane tolerance, ie. the tolerance 
		 * of the coordinate components that are not the symmetry axis. The in-plane tolerance 
		 * is always 10 times the off-plane tolerance. This value is relative to the size 
		 * of the Geometry, so 0.01 would mean 1% of the bounding box size of the Geometry. 
		 * @param value is the value to which SymmetryDetectionTolerance will be set 
		 */
		virtual	void SetSymmetryDetectionTolerance( real value ) = 0;

		/**
		 * Get the SymmetryDetectionTolerance, the tolerance used when detecting symmetry. 
		 *  Values in the ranges 1e-5 to 1e-3 will usually produce good results. This 
		 * specific tolerance corresponds to the off-plane tolerance, ie. the tolerance 
		 * of the coordinate components that are not the symmetry axis. The in-plane tolerance 
		 * is always 10 times the off-plane tolerance. This value is relative to the size 
		 * of the Geometry, so 0.01 would mean 1% of the bounding box size of the Geometry. 
		 * @return the current value of SymmetryDetectionTolerance 
		 */
		virtual	real GetSymmetryDetectionTolerance(  ) = 0;

		/**
		 * Set the flag that specifies how big "freedom" Simplygon has when it comes to 
		 * altering vertex/triangle data. Allowing more altered data means it will be 
		 * easier to create good looking LODs, but without altered data it will be easier 
		 * to keep the memory-usage in the LOD-chain more optimized. Possible options 
		 * are: <ul><li>SG_DATACREATIONPREFERENCES_ONLY_USE_ORIGINAL_DATA</li> <li>SG_DATACREATIONPREFERENCES_PREFER_ORIGINAL_DATA</li> 
		 * <li>SG_DATACREATIONPREFERENCES_PREFER_OPTIMIZED_RESULT</li></ul> Note: When 
		 * using SG_DATACREATIONPREFERENCES_USE_ONLY_ORIGINAL_DATA, the geometry being 
		 * reduced can not have a node transformation matrix other than the identity matrix. 
		 * @param value is the value to which the DataCreationPreferences flag will be 
		 * set. Value cannot be less than SG_DATACREATIONPREFERENCES_ONLY_USE_ORIGINAL_DATA. 
		 * Value cannot be greater than SG_DATACREATIONPREFERENCES_PREFER_OPTIMIZED_RESULT. 
		 *  
		 */
		virtual	void SetDataCreationPreferences( unsigned int value ) = 0;

		/**
		 * Get the flag that specifies how big "freedom" Simplygon has when it comes to 
		 * altering vertex/triangle data. Allowing more altered data means it will be 
		 * easier to create good looking LODs, but without altered data it will be easier 
		 * to keep the memory-usage in the LOD-chain more optimized. Possible options 
		 * are: <ul><li>SG_DATACREATIONPREFERENCES_ONLY_USE_ORIGINAL_DATA</li> <li>SG_DATACREATIONPREFERENCES_PREFER_ORIGINAL_DATA</li> 
		 * <li>SG_DATACREATIONPREFERENCES_PREFER_OPTIMIZED_RESULT</li></ul> Note: When 
		 * using SG_DATACREATIONPREFERENCES_USE_ONLY_ORIGINAL_DATA, the geometry being 
		 * reduced can not have a node transformation matrix other than the identity matrix. 
		 * @return the current value of the DataCreationPreferences flag. 
		 */
		virtual	unsigned int GetDataCreationPreferences(  ) = 0;

		/**
		 * Set the flag for if the reduction process should store geomorph data in the 
		 * processed GeometryData object. This data is an array with same size as the 
		 * vertexcount was before the reduction process, and will have the coordinates 
		 * for each vertex on its corresponding position on the reduced GeometryData's 
		 * surface. Can be used to give smooth transition from a highlevel LOD to a lowlevel 
		 * LOD. This array can be accessed with " GeomObj->GetCustomField( "SgMappedVertexCoords" 
		 * ) " 
		 * @param value is the bool to which GenerateGeomorphData will be set 
		 */
		virtual	void SetGenerateGeomorphData( bool value ) = 0;

		/**
		 * Get the flag for if the reduction process should store geomorph data in the 
		 * processed GeometryData object. This data is an array with same size as the 
		 * vertexcount was before the reduction process, and will have the coordinates 
		 * for each vertex on its corresponding position on the reduced GeometryData's 
		 * surface. Can be used to give smooth transition from a highlevel LOD to a lowlevel 
		 * LOD. This array can be accessed with " GeomObj->GetCustomField( "SgMappedVertexCoords" 
		 * ) " 
		 * @return the current value of the GenerateGeomorphData bool 
		 */
		virtual	bool GetGenerateGeomorphData(  ) = 0;

		//增长边界设置
		/**
		 * Set the outward move multiplier, how much the mesh is allowed to grow its silhouette 
		 *  perimeter 
		 * @param value is the value to which OutwardMoveMultiplier will be set 
		 */
		virtual	void SetOutwardMoveMultiplier( real value ) = 0;

		/**
		 * Get the outward move multiplier, how much the mesh is allowed to grow its silhouette 
		 *  perimeter 
		 * @return the current value of OutwardMoveMultiplier 
		 */
		virtual	real GetOutwardMoveMultiplier(  ) = 0;

		/**
		 * Set the inward move multiplier, how much the mesh is allowed to shrink its 
		 * silhouette perimeter 
		 * @param value is the value to which InwardMoveMultiplier will be set 
		 */
		virtual	void SetInwardMoveMultiplier( real value ) = 0;

		/**
		 * Get the inward move multiplier, how much the mesh is allowed to shrink its 
		 * silhouette perimeter 
		 * @return the current value of InwardMoveMultiplier 
		 */
		virtual	real GetInwardMoveMultiplier(  ) = 0;

		/**
		 * Sets the maximum length of the triangles edges after reduction. Can be used 
		 * to limit output triangle size. 
		 * @param value is the new value of MaxEdgeLength Value cannot be less than 0. 
		 *  
		 */
		virtual	void SetMaxEdgeLength( real value ) = 0;

		/**
		 * Gets the maximum length of the triangles edges after reduction. Can be used 
		 * to limit output triangle size. 
		 * @return the current value of MaxEdgeLength 
		 */
		virtual	real GetMaxEdgeLength(  ) = 0;

		/**
		 * Set to true and the reducer will do a much more precise way of calculating 
		 * the vertex normals. It will however be slower. Set to false and it will do 
		 * a much more rough estimation of what the vertex normals will look like, but 
		 * faster. 
		 * @param value is the value to which UseHighQualityNormalCalculation will be 
		 * set 
		 */
		virtual	void SetUseHighQualityNormalCalculation( bool value ) = 0;

		/**
		 * Get the "UseHighQualityNormalCalculation" flag, which decided if the reducer 
		 * will do a much more precise way of calculating the vertex normals or not. 
		 * @return the current value of the UseHighQualityNormalCalculation bool 
		 */
		virtual	bool GetUseHighQualityNormalCalculation(  ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to -1, all geometries 
		 * in the scene will be processed. 
		 * @param value the desired SelectionSet id Value cannot be less than -1. 
		 */
		virtual	void SetProcessSelectionSetID( rid value ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to -1, all geometries 
		 * in the scene will be processed. 
		 * @return the current SelectionSet id 
		 */
		virtual	rid GetProcessSelectionSetID(  ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If SetName is not found, all 
		 * geometries in the scene will be processed. If both ID and Name are set, ID 
		 * will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetProcessSelectionSetName( const char * value ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to NULL, all geometries 
		 * in the scene will be processed. If both ID and Name are set, ID will be used. 
		 * @return the current SelectionSet name 
		 */
		virtual	rstring GetProcessSelectionSetName(  ) = 0;

		//合并几何
		/**
		 * The processor will merge all the geometries in the scene into a single geometry 
		 *  if MergeGeometries is enabled. 
		 * @param value the desired MergeGeometries flag 
		 */
		virtual	void SetMergeGeometries( bool value ) = 0;

		/**
		 * The processor will merge all the geometries in the scene into a single geometry 
		 *  if MergeGeometries is enabled. 
		 * @return the current MergeGeometries flag 
		 */
		virtual	bool GetMergeGeometries(  ) = 0;

		/**
		 * If a name is set, the first component in the color channel will be converted 
		 * into vertex weights. 
		 * @param value the desired WeightsFromColorChannelName 
		 */
		virtual	void SetWeightsFromColorChannelName( const char * value ) = 0;

		/**
		 * If a name is set, the first component in the color channel will be converted 
		 * into vertex weights. 
		 * @return the current WeightsFromColorChannelName 
		 */
		virtual	rstring GetWeightsFromColorChannelName(  ) = 0;

		/**
		 * If an id is set, the first component in the color channel will be converted 
		 * into vertex weights. If a WeightsFromColorChannelName is set, it will override 
		 * this id. 
		 * @param value the desired WeightsFromColorChannelID 
		 */
		virtual	void SetWeightsFromColorChannelID( rid value ) = 0;

		/**
		 * If an id is set, the first component in the color channel will be converted 
		 * into vertex weights. If a WeightsFromColorChannelName is set, it will override 
		 * this id. 
		 * @return the current WeightsFromColorChannelID 
		 */
		virtual	rid GetWeightsFromColorChannelID(  ) = 0;

		//边界缩减
		/**
		 * If set, will lock the geometric border of the geometry, and not reduce at all. 
		 * This setting might be useful if the geometry needs to fit a neighbor geometry, 
		 * but will reduce overall reduction quality (geometric quality over reduction 
		 * ratio) 
		 * @param value the desired LockGeometricBorder flag 
		 */
		virtual	void SetLockGeometricBorder( bool value ) = 0;

		/**
		 * If set, will lock the geometric border of the geometry, and not reduce at all. 
		 * This setting might be useful if the geometry needs to fit a neighbor geometry, 
		 * but will reduce overall reduction quality (geometric quality over reduction 
		 * ratio) 
		 * @return the current LockGeometricBorder flag 
		 */
		virtual	bool GetLockGeometricBorder(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IReductionSettings
	 */
	typedef CountedPointer<IReductionSettings> spReductionSettings;

	/// Conditional build define for the interface IReductionSettings
	#define InterfaceDefined_SimplygonSDK_IReductionSettings 1


	/**
	 * Settings for the normal recalculation 
	 */
	class INormalCalculationSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if INormalCalculationSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("INormalCalculationSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a INormalCalculationSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a INormalCalculationSettings pointer
		 * @return a pointer to the INormalCalculationSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static INormalCalculationSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("INormalCalculationSettings") )
				return static_cast<INormalCalculationSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set ReplaceNormals flag. If set, it will generate new normals for the Geometry, 
		 *  and if not set, it will keep the normals from the original normal-set (or 
		 * not add normals at all if none were present). Please note that if the original 
		 * geometry contains normals, the normal repairer will replace invalid normals 
		 * even if ReplaceNormals is set to false. 
		 * @param value is the value to which the ReplaceNormals bool is to be set 
		 */
		virtual	void SetReplaceNormals( bool value ) = 0;

		/**
		 * Get ReplaceNormals flag. If set, it will generate new normals for the Geometry, 
		 *  and if not set, it will keep the normals from the original normal-set (or 
		 * not add normals at all if none were present). Please note that if the original 
		 * geometry contains normals, the normal repairer will replace invalid normals 
		 * even if ReplaceNormals is set to false. 
		 * @return the current value of the ReplaceNormals bool 
		 */
		virtual	bool GetReplaceNormals(  ) = 0;

		/**
		 * Set ReplaceTangents flag. If set, it will generate new tangents and bitangents 
		 * for the Geometry, and if not set, it will keep the new tangents and bitangents 
		 * from the original normal-set (or not add tangents and bitangents at all if 
		 * none were present). 
		 * @param value is the value to which the ReplaceTangents bool is to be set 
		 */
		virtual	void SetReplaceTangents( bool value ) = 0;

		/**
		 * Get ReplaceTangents flag. If set, it will generate new tangents and bitangents 
		 * for the Geometry, and if not set, it will keep the new tangents and bitangents 
		 * from the original normal-set (or not add tangents and bitangents at all if 
		 * none were present). 
		 * @return the current value of the ReplaceTangents bool 
		 */
		virtual	bool GetReplaceTangents(  ) = 0;

		/**
		 * Set HardEdgeAngleInRadians. If the angle between two triangles are above this 
		 * value, the normals will not be smooth over the edge between those two triangles. 
		 * Set in radians. 
		 * @param value is the value to which HardEdgeAngleInRadians is to be set 
		 */
		virtual	void SetHardEdgeAngleInRadians( real value ) = 0;

		/**
		 * Get HardEdgeAngleInRadians. If the angle between two triangles are above this 
		 * value, the normals will not be smooth over the edge between those two triangles. 
		 * Set in radians. 
		 * @return the current value of HardEdgeAngleInRadians 
		 */
		virtual	real GetHardEdgeAngleInRadians(  ) = 0;

		/**
		 * Set the RepairInvalidNormals flag If set, normals that are invalid will be 
		 * replaced. All others are left intact. Bad normals are those that are either 
		 * Zero length, or simply points away from the surface 
		 * @param value is the value to which RepairInvalidNormals is to be set 
		 */
		virtual	void SetRepairInvalidNormals( bool value ) = 0;

		/**
		 * Get the RepairInvalidNormals flag If set, normals that are invalid will be 
		 * replaced. All others are left intact. Bad normals are those that are either 
		 * Zero length, or simply points away from the surface 
		 */
		virtual	bool GetRepairInvalidNormals(  ) = 0;

		/**
		 * Get the ScaleByArea flag. If set, the influence of a triangle normal upon the 
		 * vertex normals is scaled by the area of the triangle. 
		 * @return the current value of the ScaleByArea bool 
		 */
		virtual	bool GetScaleByArea(  ) = 0;

		/**
		 * Set the ScaleByArea flag. If set, the influence of a triangle normal upon the 
		 * vertex normals is scaled by the area of the triangle. 
		 * @param value is the value to which the ScaleByArea bool is to be set 
		 */
		virtual	void SetScaleByArea( bool value ) = 0;

		/**
		 * Get the ScaleByAngle flag. If set, the influence of a triangle normal upon 
		 * the vertex normals is scaled by the angle of the corner at the vertex. 
		 * @return the current value of the ScaleByAngle bool 
		 */
		virtual	bool GetScaleByAngle(  ) = 0;

		/**
		 * Set the ScaleByAngle flag. If set, the influence of a triangle normal upon 
		 * the vertex normals is scaled by the angle of the corner at the vertex. 
		 * @param value is the value to which the ScaleByAngle bool is to be set 
		 */
		virtual	void SetScaleByAngle( bool value ) = 0;

		/**
		 * Get the SnapNormalsToFlatSurfaces flag. If set, perfectly flat shaded surfaces 
		 * will be created, where possible. Normals adjacent to perfectly geometrically 
		 * flat surfaces will be recalculated to have the same direction as for those 
		 * of the adjacent flat surface. 
		 * @return the current value of the SnapNormalsToFlatSurfaces 
		 */
		virtual	bool GetSnapNormalsToFlatSurfaces(  ) = 0;

		/**
		 * Set the SnapNormalsToFlatSurfaces flag. If set, perfectly flat shaded surfaces 
		 * will be created, where possible. Normals adjacent to perfectly geometrically 
		 * flat surfaces will be recalculated to have the same direction as for those 
		 * of the adjacent flat surface. 
		 * @param value to use for SnapNormalsToFlatSurfaces 
		 */
		virtual	void SetSnapNormalsToFlatSurfaces( bool value ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetSnapNormalsToFlatSurfaces GetSnapNormalsToFlatSurfaces() \endlink 
		 * instead. Use GetSnapNormalsToFlatSurfaces instead 
		 */
		virtual	SGDEPRECATED bool GetDetectPrimitiveNormals(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link SetSnapNormalsToFlatSurfaces SetSnapNormalsToFlatSurfaces() \endlink 
		 * instead. Use SetSnapNormalsToFlatSurfaces instead 
		 * @param detectPrimitiveNormals is the value to which the detectPrimitiveNormals 
		 * bool is to be set 
		 */
		virtual	SGDEPRECATED void SetDetectPrimitiveNormals( bool detectPrimitiveNormals ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an INormalCalculationSettings
	 */
	typedef CountedPointer<INormalCalculationSettings> spNormalCalculationSettings;

	/// Conditional build define for the interface INormalCalculationSettings
	#define InterfaceDefined_SimplygonSDK_INormalCalculationSettings 1


	/**
	 * Settings for the Simplygon mapping classes These settings control the properties 
	 *  of the mapping image that will be generated in the current simplification processor 
	 *  (reducer/remesher etc). The mapping image will be used when casting a new texture 
	 *  on the simplified geometry. In the settings object, the user is able to set 
	 * the dimensions of the image (width/height) as well as whether the LOD should 
	 * have new texture coordinates and some parameters for how they should be generated 
	 * and all other settings regarding the mapping image. 
	 */
	class IMappingImageSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IMappingImageSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IMappingImageSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IMappingImageSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IMappingImageSettings pointer
		 * @return a pointer to the IMappingImageSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IMappingImageSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IMappingImageSettings") )
				return static_cast<IMappingImageSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the number of input materials used in the original geometry for mapping 
		 * to multiple output materials. This needs to be set before you can set any specific 
		 * in-out material mapping. 0 signifies that no in-out material mapping is used, 
		 * ie. the process will produce one resulting mapping image. 
		 * @param value is the number of inputs Value cannot be less than 0. Value cannot 
		 * be greater than SG_MAXIMUM_MAPPING_IMAGE_INPUT_MATERIAL_COUNT. 
		 */
		virtual	void SetInputMaterialCount( unsigned int value ) = 0;

		/**
		 * Get the number of input materials set earlier. 0 signifies that no in-out material 
		 *  mapping is used. 
		 * @return the current value of InputMaterialCount 
		 */
		virtual	unsigned int GetInputMaterialCount(  ) = 0;

		/**
		 * Set the number of output mapping images that are to be generated. If this is 
		 * set to more than 1, the material mapping also needs to be set. 
		 * @param value is the number of outputs Value cannot be less than 1. Value cannot 
		 * be greater than SG_MAXIMUM_MAPPING_IMAGE_OUTPUT_MATERIAL_COUNT. 
		 */
		virtual	void SetOutputMaterialCount( unsigned int value ) = 0;

		/**
		 * Get the number of output mapping images that are to be generated. 
		 * @return the current value of OutputMaterialCount 
		 */
		virtual	unsigned int GetOutputMaterialCount(  ) = 0;

		/**
		 * Set the material mapping for mat_id, meaning what material of the generated 
		 * LOD mat_id will be baked into. Both InputMaterialCount and OutputMaterialCount 
		 * need to be set for this mapping to work, and all original materials need to 
		 * be mapped to an existing output id. 
		 * @param InMaterialId is the input material id 
		 * @param OutMaterialId is the output material id the corresponding input is to 
		 * be mapped to 
		 */
		virtual	void SetInputOutputMaterialMapping( unsigned int InMaterialId , unsigned int OutMaterialId ) = 0;

		/**
		 * Get the previously set material mapping for mat_id. 
		 * @param InMaterialId is the input material id 
		 * @return the currently mapped output material for InMaterialId. -1 means mapping 
		 * is not set. 
		 */
		virtual	int GetInputOutputMaterialMapping( unsigned int InMaterialId ) = 0;

		/**
		 * Set the width of the texture to use. If called with an id, it will set the 
		 * width of that specific texture if using multiple mapping image outputs. 
		 * @param value is the value to which Width will be set 
		 */
		virtual	void SetWidth( unsigned int value ) = 0;

		/**
		 * Set the width of the texture to use. If called with an id, it will set the 
		 * width of that specific texture if using multiple mapping image outputs. 
		 * @param id the texture id 
		 * @param value is the value to which Width will be set 
		 */
		virtual	void SetWidth( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the width of the texture to use. If called with an id, it will get the 
		 * width of that specific texture if using multiple mapping image outputs. 
		 * @return the current value of Width 
		 */
		virtual	unsigned int GetWidth(  ) = 0;

		/**
		 * Get the width of the texture to use. If called with an id, it will get the 
		 * width of that specific texture if using multiple mapping image outputs. 
		 * @param id the texture id 
		 * @return the current value of Width 
		 */
		virtual	unsigned int GetWidth( unsigned int id ) = 0;

		/**
		 * Set the name of the mapping image generated by the processor. If called with 
		 * an id, it will set the name of that specific mapping image if using multiple 
		 * mapping image outputs. 
		 * @param name the name of the mapping image 
		 */
		virtual	void SetMappingImageName( const char * name ) = 0;

		/**
		 * Set the name of the mapping image generated by the processor. If called with 
		 * an id, it will set the name of that specific mapping image if using multiple 
		 * mapping image outputs. 
		 * @param id the texture id 
		 * @param name the name of the mapping image 
		 */
		virtual	void SetMappingImageName( unsigned int id , const char * name ) = 0;

		/**
		 * Set the name of the mapping image generated by the processor. If called with 
		 * an id, it will set the name of that specific mapping image if using multiple 
		 * mapping image outputs. 
		 * @return the current name 
		 */
		virtual	rstring GetMappingImageName(  ) = 0;

		/**
		 * Set the name of the mapping image generated by the processor. If called with 
		 * an id, it will set the name of that specific mapping image if using multiple 
		 * mapping image outputs. 
		 * @param id the texture id 
		 * @return the current name 
		 */
		virtual	rstring GetMappingImageName( unsigned int id ) = 0;

		/**
		 * Determines whether to replace already existing mapping images in the scene 
		 * @param value the new value 
		 */
		virtual	void SetReplaceMappingImages( bool value ) = 0;

		/**
		 * Determines whether to replace already existing mapping images in the scene 
		 * @return the current value 
		 */
		virtual	bool GetReplaceMappingImages(  ) = 0;

		/**
		 * Set the height of the texture to use. If called with an id, it will set the 
		 * height of that specific texture if using multiple mapping image outputs. 
		 * @param value is the value to which Height will be set 
		 */
		virtual	void SetHeight( unsigned int value ) = 0;

		/**
		 * Set the height of the texture to use. If called with an id, it will set the 
		 * height of that specific texture if using multiple mapping image outputs. 
		 * @param id the texture id 
		 * @param value is the value to which Height will be set 
		 */
		virtual	void SetHeight( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the height of the texture to use. If called with an id, it will get the 
		 * height of that specific texture if using multiple mapping image outputs. 
		 * @return the current value of Height 
		 */
		virtual	unsigned int GetHeight(  ) = 0;

		/**
		 * Get the height of the texture to use. If called with an id, it will get the 
		 * height of that specific texture if using multiple mapping image outputs. 
		 * @param id the texture id 
		 * @return the current value of Height 
		 */
		virtual	unsigned int GetHeight( unsigned int id ) = 0;

		/**
		 * Set the multi-sampling level. Values 1-8 are accepted. If called with an id, 
		 * it will set the multi-sampling level of that specific texture if using multiple 
		 * mapping image outputs. 
		 * @param value is the value to which MultiSamplingLevel will be set 
		 */
		virtual	void SetMultisamplingLevel( unsigned int value ) = 0;

		/**
		 * Set the multi-sampling level. Values 1-8 are accepted. If called with an id, 
		 * it will set the multi-sampling level of that specific texture if using multiple 
		 * mapping image outputs. 
		 * @param id the texture id 
		 * @param value is the value to which MultiSamplingLevel will be set 
		 */
		virtual	void SetMultisamplingLevel( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the multi-sampling level. Values 1-8 are accepted. If called with an id, 
		 * it will set the multi-sampling level of that specific texture if using multiple 
		 * mapping image outputs. 
		 * @return the current value of MultiSamplingLevel 
		 */
		virtual	unsigned int GetMultisamplingLevel(  ) = 0;

		/**
		 * Get the multi-sampling level. Values 1-8 are accepted. If called with an id, 
		 * it will set the multi-sampling level of that specific texture if using multiple 
		 * mapping image outputs. 
		 * @param id the texture id 
		 * @return the current value of MultiSamplingLevel 
		 */
		virtual	unsigned int GetMultisamplingLevel( unsigned int id ) = 0;

		/**
		 * Set the minimum number of pixels between charts. If called with an id, it will 
		 * set the width of that specific texture if using multiple mapping image outputs. 
		 * The gutter space has to be set to a value larger than 0. 
		 * @param value is the value to which GutterSpace will be set 
		 */
		virtual	void SetGutterSpace( unsigned int value ) = 0;

		/**
		 * Set the minimum number of pixels between charts. If called with an id, it will 
		 * set the width of that specific texture if using multiple mapping image outputs. 
		 * The gutter space has to be set to a value larger than 0. 
		 * @param id the texture id 
		 * @param value is the value to which GutterSpace will be set 
		 */
		virtual	void SetGutterSpace( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the minimum number of pixels between charts. If called with an id, it will 
		 * set the width of that specific texture if using multiple mapping image outputs. 
		 * @return the current value of GutterSpace 
		 */
		virtual	unsigned int GetGutterSpace(  ) = 0;

		/**
		 * Get the minimum number of pixels between charts. If called with an id, it will 
		 * set the width of that specific texture if using multiple mapping image outputs. 
		 * @param id the texture id 
		 * @return the current value of GutterSpace 
		 */
		virtual	unsigned int GetGutterSpace( unsigned int id ) = 0;

		/**
		 * Set the maximum number of layers in the mapping image. If a remeshed geometry 
		 * has a lot of transparent triangles, use a higher number to be able to find 
		 * intersections on the inside of parts of the geometry. This setting only has 
		 * effect in the remeshing, when running reduction the number of layers is always 
		 * 1. 
		 * @param value the desired number of layers in the mapping image 
		 */
		virtual	void SetMaximumLayers( unsigned int value ) = 0;

		/**
		 * Get the maximum number of layers in the mapping image. If a remeshed geometry 
		 * has a lot of transparent triangles, use a higher number to be able to find 
		 * intersections on the inside of parts of the geometry. This setting only has 
		 * effect in the remeshing, when running reduction the number of layers is always 
		 * 1. 
		 * @return the current number of layers in the mapping image 
		 */
		virtual	unsigned int GetMaximumLayers(  ) = 0;

		/**
		 * Set the AllowTransparencyMapping flag. If true, the remeshing processor will 
		 * allow mapped transparency in the generated mapping image. This allows material 
		 * casting where gaps filled by the remeshing processor will be cast as transparent. 
		 * @param value the desired value of AllowTransparencyMapping 
		 */
		virtual	void SetAllowTransparencyMapping( bool value ) = 0;

		/**
		 * Get the AllowTransparencyMapping flag. If true, the remeshing processor will 
		 * allow mapped transparency in the generated mapping image. This allows material 
		 * casting where gaps filled by the remeshing processor will be cast as transparent. 
		 * @return the current value of AllowTransparencyMapping 
		 */
		virtual	bool GetAllowTransparencyMapping(  ) = 0;

		/**
		 * Set the GenerateMappingImage flag. If set, the remesher will create a mapping 
		 * image, that maps triangles on the reduced geometry back to triangles on the 
		 * original geometry. The texture is insert into the MappingImage object. The 
		 * image data will contain two fields: <li> <ul> * ReRidArray, "TriangleIds", 
		 * TupleSize:1, The id of the triangle </ul> <ul> * ReUnsignedShortArra, "BarycentricCoords", 
		 * TupleSize:2, Items A & B of the barycentric coordinate on the triangle. </ul> 
		 * </li> The user can control the size of the mapped texture using the values 
		 * MappingTextureWidth and MappingTextureHeight 
		 * @param value the desired GenerateMappingImage flag 
		 */
		virtual	void SetGenerateMappingImage( bool value ) = 0;

		/**
		 * Get the GenerateMappingImage flag. If set, the remesher will create a mapping 
		 * image, that maps triangles on the reduced geometry back to triangles on the 
		 * original geometry. The texture is insert into the MappingImage object. The 
		 * image data will contain two fields: <li> <ul> * ReRidArray, "TriangleIds", 
		 * TupleSize:1, The id of the triangle </ul> <ul> * ReUnsignedShortArra, "BarycentricCoords", 
		 * TupleSize:2, Items A & B of the barycentric coordinate on the triangle. </ul> 
		 * </li> The user can control the size of the mapped texture using the values 
		 * MappingTextureWidth and MappingTextureHeight 
		 * @return the current GenerateMappingImage flag 
		 */
		virtual	bool GetGenerateMappingImage(  ) = 0;

		/**
		 * Sets the GenerateTexCoords flag. If true, generates a new texcoords field at 
		 * the position specified by TexCoordLevel. Will be ignored and always generate 
		 * texcoords if none are available in the Geometry, at same time as GenerateMappingImage 
		 * is enabled. 
		 * @param value the desired GenerateTexCoords flag 
		 */
		virtual	void SetGenerateTexCoords( bool value ) = 0;

		/**
		 * Gets the GenerateTexCoords flag. If true, generates a new texcoords field at 
		 * the position specified by TexCoordLevel. Will be ignored and always generate 
		 * texcoords if none are available in the Geometry, at same time as GenerateMappingImage 
		 * is enabled. 
		 * @return the current GenerateTexCoords flag 
		 */
		virtual	bool GetGenerateTexCoords(  ) = 0;

		/**
		 * Sets the GenerateTangents flag. If true, generates a new tangent space fields 
		 * at position specified by TexCoordLevel, based on the vertex normals and texture 
		 * coordinates. Tangent space fields will always be created if missing, and GenerateMappingImage 
		 *  is enabled. If GenerateTexCoords is enabled, new tangents will always be generated. 
		 * @param value the desired GenerateTangents flag 
		 */
		virtual	void SetGenerateTangents( bool value ) = 0;

		/**
		 * Gets the GenerateTangents flag. If true, generates a new tangent space fields 
		 * at position specified by TexCoordLevel, based on the vertex normals and texture 
		 * coordinates. Tangent space fields will always be created if missing, and GenerateMappingImage 
		 *  is enabled. If GenerateTexCoords is enabled, new tangents will always be generated. 
		 * @return the current GenerateTangents flag 
		 */
		virtual	bool GetGenerateTangents(  ) = 0;

		/**
		 * Set the texture coordinate level used for mapping image and texture generation. 
		 *  Default is 0. If the TexCoordLevelName has been set, the TexCoordLevel is 
		 * not used. 
		 * @param value the desired texture coordinate level 
		 */
		virtual	void SetTexCoordLevel( rid value ) = 0;

		/**
		 * Get the texture coordinate level used for mapping image and texture generation. 
		 *  Default is 0. If the TexCoordLevelName has been set, the TexCoordLevel is 
		 * not used. 
		 * @return the current texture coordinate level 
		 */
		virtual	rid GetTexCoordLevel(  ) = 0;

		/**
		 * Set the texture coordinate level name used for mapping image and texture generation. 
		 *  If new UVs are created, they will replace the old coordinates in the texture 
		 * channel with the TexCoordLevelName. If no texture level with this name exists. 
		 * It will be created. If the TexCoordLevelName is not set, the TexCoordLevel 
		 * (index) will be used instead. 
		 * @param value is the TexCoord level name 
		 * @return the current texture coordinate level name 
		 */
		virtual	void SetTexCoordLevelName( const char * value ) = 0;

		/**
		 * Set the texture coordinate level name used for mapping image and texture generation. 
		 *  If new UVs are created, they will replace the old coordinates in the texture 
		 * channel with the TexCoordLevelName. If no texture level with this name exists. 
		 * It will be created. If the TexCoordLevelName is not set, the TexCoordLevel 
		 * (index) will be used instead. 
		 * @return the current texture coordinate level name 
		 */
		virtual	rstring GetTexCoordLevelName(  ) = 0;

		/**
		 * Set the UseFullRetexturing flag. If set, all original texture sets are replaced 
		 *  by the one generated for the mapping image, and the TexCoordLevel value is 
		 * always set to 0. This flag has no effect on the remeshing, which always does 
		 * a full retexturing. 
		 * @param value the desired UseFullRetexturing flag 
		 */
		virtual	void SetUseFullRetexturing( bool value ) = 0;

		/**
		 * Set/Get the UseFullRetexturing flag. If set, all original texture sets are 
		 * replaced by the one generated for the mapping image, and the TexCoordLevel 
		 * value is always set to 0. This flag has no effect on the remeshing, which always 
		 * does a full retexturing. 
		 * @return the current UseFullRetexturing flag 
		 */
		virtual	bool GetUseFullRetexturing(  ) = 0;

		/**
		 * Set the UseAutomaticTextureSize flag. If true, then texture sizes will be computed 
		 *  for the reduced mesh depending on its pixel size on screen. 
		 * @param value the desired flag 
		 */
		virtual	void SetUseAutomaticTextureSize( bool value ) = 0;

		/**
		 * Set the UseAutomaticTextureSize flag. If true, then texture sizes will be computed 
		 *  for the reduced mesh depending on its pixel size on screen. 
		 * @return the current flag 
		 */
		virtual	bool GetUseAutomaticTextureSize(  ) = 0;

		/**
		 * Set a texture dimension length multiplier for the automatic texture size. 
		 * @param value the desired multiplier 
		 */
		virtual	void SetAutomaticTextureSizeMultiplier( real value ) = 0;

		/**
		 * Get the current automatic texture size multiplier. 
		 * @return the current multiplier 
		 */
		virtual	real GetAutomaticTextureSizeMultiplier(  ) = 0;

		/**
		 * If automatic_texture_size is enabled, then force the texture sizes to be a 
		 * power of 2 
		 * @param value the desired flag 
		 */
		virtual	void SetForcePower2Texture( bool value ) = 0;

		/**
		 * If automatic_texture_size is enabled, then force the texture sizes to be a 
		 * power of 2 
		 * @return the current flag 
		 */
		virtual	bool GetForcePower2Texture(  ) = 0;

		/**
		 * Determines which method to use when generating the texture coordinates. SG_TEXCOORDGENERATORTYPE_PARAMETERIZER 
		 *  generate from scratch based on the geometry. SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR 
		 *  generate based on the original texture coordinates. 
		 * @param value is the TexCoordGeneratorType value to use 
		 */
		virtual	void SetTexCoordGeneratorType( unsigned int value ) = 0;

		/**
		 * Determines which method to use when generating the texture coordinates. SG_TEXCOORDGENERATORTYPE_PARAMETERIZER 
		 *  generate from scratch based on the geometry. SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR 
		 *  generate based on the original texture coordinates. 
		 * @return the current SG_TEXCOORDGENERATORTYPE_[...] value. 
		 */
		virtual	unsigned int GetTexCoordGeneratorType(  ) = 0;

		/**
		 * Determines which method to use when aggregating the UVs. SG_CHARTAGGREGATORMODE_TEXTURESIZEPROPORTIONS 
		 *  - Aggregated UV charts will be scaled to keep their relative pixel density 
		 * relative to all other UV charts. The user can still set the size of the output 
		 * texture maps. SG_CHARTAGGREGATORMODE_SURFACEAREA - Aggregated UV charts will 
		 * have their size set based on its actual geometrical size. SG_CHARTAGGREGATORMODE_ORIGINALPIXELDENSITY 
		 *  - The combined atlas will be resized to fit each chart so that all charts 
		 * retain the same amount of pixels as they originally had. This will override 
		 * any manually set texture size. SG_CHARTAGGREGATORMODE_UVSIZEPROPORTIONS - Aggregated 
		 * UV charts will have their size set based on its original UV size, disregarding 
		 * the size of the texture they are used in."); 
		 * @param value is the ChartAggregaotrMode value to use 
		 */
		virtual	void SetChartAggregatorMode( unsigned int value ) = 0;

		/**
		 * Determines which method to use when aggregating the UVs. SG_CHARTAGGREGATORMODE_TEXTURESIZEPROPORTIONS 
		 *  - Aggregated UV charts will be scaled to keep their relative pixel density 
		 * relative to all other UV charts. The user can still set the size of the output 
		 * texture maps. SG_CHARTAGGREGATORMODE_SURFACEAREA - Aggregated UV charts will 
		 * have their size set based on its actual geometrical size. SG_CHARTAGGREGATORMODE_ORIGINALPIXELDENSITY 
		 *  - The combined atlas will be resized to fit each chart so that all charts 
		 * retain the same amount of pixels as they originally had. This will override 
		 * any manually set texture size. SG_CHARTAGGREGATORMODE_UVSIZEPROPORTIONS - Aggregated 
		 * UV charts will have their size set based on its original UV size, disregarding 
		 * the size of the texture they are used in."); 
		 * @return the current SG_CHARTAGGREGATORMODE_[...] value. 
		 */
		virtual	unsigned int GetChartAggregatorMode(  ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_PARAMETERIZER. 
		 *  The maximum allowed texture stretch. Range 0->1 
		 * @param value the desired maximum allowed texture stretch 
		 */
		virtual	void SetParameterizerMaxStretch( real value ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_PARAMETERIZER. 
		 *  The maximum allowed texture stretch. Range 0->1 
		 * @return the current maximum allowed texture stretch 
		 */
		virtual	real GetParameterizerMaxStretch(  ) = 0;

		/**
		 * Scales the resulting UVs based on the vertex weights. Larger weights mean more 
		 * detailed texture coordinates. 
		 * @param value the desired flag 
		 */
		virtual	void SetUseVertexWeights( bool value ) = 0;

		/**
		 * Scales the resulting UVs based on the vertex weights. Larger weights mean more 
		 * detailed texture coordinates. 
		 * @return the current flag 
		 */
		virtual	bool GetUseVertexWeights(  ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  Set the texture coordinate level to get charts from. Default is 0. If the 
		 * ChartAggregatorOriginalTexCoordLevelName ha been set, the ChartAggregatorOriginalTexCoordLevel 
		 * is not used. 
		 * @param value the desired texture coordinate level 
		 */
		virtual	void SetChartAggregatorOriginalTexCoordLevel( rid value ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  Set the texture coordinate level to get charts from. Default is 0. If the 
		 * ChartAggregatorOriginalTexCoordLevelName ha been set, the ChartAggregatorOriginalTexCoordLevel 
		 * is not used. 
		 * @return the current texture coordinate level 
		 */
		virtual	rid GetChartAggregatorOriginalTexCoordLevel(  ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  Set the name of the texture coordinate level to get charts from. If not set, 
		 * it will use the ChartAggregatorOriginalTexCoordLevel (index) instead. 
		 * @param value the desired texture coordinate level name 
		 */
		virtual	void SetChartAggregatorOriginalTexCoordLevelName( const char * value ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  Set the name of the texture coordinate level to get charts from. If not set, 
		 * it will use the ChartAggregatorOriginalTexCoordLevel (index) instead. 
		 * @return the current texture coordinate level name 
		 */
		virtual	rstring GetChartAggregatorOriginalTexCoordLevelName(  ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  If set, charts that are overlapping in the original texture coords will be 
		 * separated. 
		 * @param value is the value to which ChartAggregatorSeparateOverlappingCharts 
		 * is set 
		 */
		virtual	void SetChartAggregatorSeparateOverlappingCharts( bool value ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  If set, charts that are overlapping in the original texture coords will be 
		 * separated. 
		 * @return the current flag value 
		 */
		virtual	bool GetChartAggregatorSeparateOverlappingCharts(  ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  Set ChartAggregatorKeepOriginalChartProportionsFromChannel to an SG_MATERIAL_CHANNEL_[...]. 
		 *  This channel determines which texture channel to look at when determining 
		 * which chart proportions to keep. The default value is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @param value the new ChartAggregatorKeepOriginalChartProportionsFromChannel 
		 * value 
		 */
		virtual	void SetChartAggregatorOriginalChartProportionsChannel( const char * value ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  Set ChartAggregatorKeepOriginalChartProportionsFromChannel to an SG_MATERIAL_CHANNEL_[...]. 
		 *  This channel determines which texture channel to look at when determining 
		 * which chart proportions to keep. The default value is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @return the current ChartAggregatorKeepOriginalChartProportionsFromChannel 
		 * value 
		 */
		virtual	rstring GetChartAggregatorOriginalChartProportionsChannel(  ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_PARAMETERIZER. 
		 *  The importance value for trying to have as large and few charts as possible. 
		 * The closer the value is to 1.0f, the slower it is. Having 0.0f means no extra 
		 * focus will be put on reducing the number of charts. Having value 1.0f means 
		 * maximum focus will be put on reducing the number of charts and thereby increasing 
		 * the size of the charts. 
		 * @param value the desired large charts importance value 
		 */
		virtual	void SetParameterizerLargeChartsImportance( real value ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_PARAMETERIZER. 
		 *  The importance value for trying to have as large and few charts as possible. 
		 * The closer the value is to 1.0f, the slower it is. Having 0.0f means no extra 
		 * focus will be put on reducing the number of charts. Having value 1.0f means 
		 * maximum focus will be put on reducing the number of charts and thereby increasing 
		 * the size of the charts. 
		 * @return the current large charts importance value 
		 */
		virtual	real GetParameterizerLargeChartsImportance(  ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  If ChartAggregatorLockUVRotation is enabled, only the original UV rotation 
		 * will be used. Otherwise 4 rotations are used (90 degree rotations). 
		 * @return the current LockUVRotation 
		 */
		virtual	bool GetChartAggregatorLockUVRotation(  ) = 0;

		/**
		 * Only used when the TexCoordGeneratorType is SG_TEXCOORDGENERATORTYPE_CHARTAGGREGATOR. 
		 *  If ChartAggregatorLockUVRotation is enabled, only the original UV rotation 
		 * will be used. Otherwise 4 rotations are used (90 degree rotations). 
		 * @param value the new LockUVRotation 
		 */
		virtual	void SetChartAggregatorLockUVRotation( bool value ) = 0;

		/**
		 * If enabled, will aggregate all the valid original UVs. If there are invalid 
		 * original UVs, those parts will receive new parameterized UVs. 
		 * @return the current flag 
		 */
		virtual	bool GetOnlyParameterizeInvalidUVs(  ) = 0;

		/**
		 * If enabled, will aggregate all the valid original UVs. If there are invalid 
		 * original UVs, those parts will receive new parameterized UVs. 
		 * @param value the new flag 
		 */
		virtual	void SetOnlyParameterizeInvalidUVs( bool value ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IMappingImageSettings
	 */
	typedef CountedPointer<IMappingImageSettings> spMappingImageSettings;

	/// Conditional build define for the interface IMappingImageSettings
	#define InterfaceDefined_SimplygonSDK_IMappingImageSettings 1


	/**
	 * Visibility settings contains the parameters for how to compute the visibility 
	 * of a scene and how to use it, both in the IReductionProcessor and the IRemeshingProcessor. 
	 *  The visibility information is computed for each part of the mesh determined 
	 * by the visibility from the selected ISceneCameras. Cameras are selected using 
	 * the CameraSelectionSetID. The visibility can be used to guide the reducer and/or 
	 * texture coordinates generator (parameterizer) with the flags UseVisibilityWeightsInReducer 
	 * respective UseVisibilityWeightsInTexcoordGenerator. There is also an option 
	 * to completely throw away triangles that are deemed not visible, using the CullOccludedGeometry 
	 * flag. 
	 */
	class IVisibilitySettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IVisibilitySettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IVisibilitySettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IVisibilitySettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IVisibilitySettings pointer
		 * @return a pointer to the IVisibilitySettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IVisibilitySettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IVisibilitySettings") )
				return static_cast<IVisibilitySettings*>(ptr);
			return NULL;
			}

		/**
		 * If set to true, visibility weights will be used in the reducer. 
		 * @param value the desired flag value 
		 */
		virtual	void SetUseVisibilityWeightsInReducer( bool value ) = 0;

		/**
		 * If set to true, visibility weights will be used in the reducer. 
		 * @return the current UseVisibilityWeightsInReducer value 
		 */
		virtual	bool GetUseVisibilityWeightsInReducer(  ) = 0;

		/**
		 * If set to true, visibility weights will be used when generating new texture 
		 * coordinates. 
		 * @param value the new UseVisibilityWeightsInTexcoordGenerator value 
		 */
		virtual	void SetUseVisibilityWeightsInTexcoordGenerator( bool value ) = 0;

		/**
		 * If set to true, visibility weights will be used when generating new texture 
		 * coordinates. 
		 * @return the current UseVisibilityWeightsInTexcoordGenerator value 
		 */
		virtual	bool GetUseVisibilityWeightsInTexcoordGenerator(  ) = 0;

		/**
		 * Set how aggressively reducer should handle low visibility weights. 
		 * @param value Power of influence (default 1.0). 
		 */
		virtual	void SetVisibilityWeightsPower( real value ) = 0;

		/**
		 * How aggressively reducer should handle low visibility weights. 
		 * @return the current VisibilityWeightsPower value 
		 */
		virtual	real GetVisibilityWeightsPower(  ) = 0;

		/**
		 * If set to true, geometry that is not visible will be removed. 
		 * @param value the new CullOccludedGeometry value 
		 */
		virtual	void SetCullOccludedGeometry( bool value ) = 0;

		/**
		 * If set to true, geometry that is not visible will be removed. 
		 * @return the current CullOccludedGeometry value 
		 */
		virtual	bool GetCullOccludedGeometry(  ) = 0;

		/**
		 * Set the ForceVisibilityCalculation if true, visibility weights will definitely 
		 * be computed for the geometry, even if the visibility isn't specifically being 
		 * used for reduction/materialLOD/culling according to the VisibilitySettings. 
		 * @param value the new ForceVisibilityCalculation value 
		 */
		virtual	void SetForceVisibilityCalculation( bool value ) = 0;

		/**
		 * Get the ForceVisibilityCalculation if true, visibility weights will definitely 
		 * be computed for the geometry, even if the visibility isn't specifically being 
		 * used for reduction/materialLOD/culling according to the VisibilitySettings. 
		 * @return the current ForceVisibilityCalculation value 
		 */
		virtual	bool GetForceVisibilityCalculation(  ) = 0;

		/**
		 * Set whether triangles should be regarded visible when viewed from the back. 
		 * @param value the desired flag value 
		 */
		virtual	void SetUseBackfaceCulling( bool value ) = 0;

		/**
		 * Get whether triangles should be regarded visible when viewed from the back. 
		 * @return the current flag value 
		 */
		virtual	bool GetUseBackfaceCulling(  ) = 0;

		/**
		 * CameraSelectionSetID is the ID of the selection set containing all the scene 
		 * cameras used when computing visibility. If set to -1, all cameras in the scene 
		 * will be used. 
		 * @param value the desired SelectionSetID value 
		 */
		virtual	void SetCameraSelectionSetID( int value ) = 0;

		/**
		 * CameraSelectionSetID is the ID of the selection set containing all the scene 
		 * cameras used when computing visibility. If set to -1, all cameras in the scene 
		 * will be used. 
		 * @return the current SelectionSetID value 
		 */
		virtual	int GetCameraSelectionSetID(  ) = 0;

		/**
		 * CameraSelectionSetID is the name of the selection set containing all the scene 
		 * cameras used when computing visibility. If set to NULL, all cameras in the 
		 * scene will be used. If both ID and Name are set, ID will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetCameraSelectionSetName( const char * value ) = 0;

		/**
		 * CameraSelectionSetID is the name of the selection set containing all the scene 
		 * cameras used when computing visibility. If set to NULL, all cameras in the 
		 * scene will be used. If both ID and Name are set, ID will be used. 
		 */
		virtual	rstring GetCameraSelectionSetName(  ) = 0;

		/**
		 * OccluderSelectionSetID is the ID of the selection set containing all the scene 
		 * meshes that should occlude the scene when calculating visibility. If set to 
		 * -1, no occluders will be used. 
		 * @param value the desired OccluderSelectionSetID value 
		 */
		virtual	void SetOccluderSelectionSetID( int value ) = 0;

		/**
		 * OccluderSelectionSetID is the ID of the selection set containing all the scene 
		 * meshes that should occlude the scene when calculating visibility. If set to 
		 * -1, no occluders will be used. 
		 * @return the current OccluderSelectionSetID value 
		 */
		virtual	int GetOccluderSelectionSetID(  ) = 0;

		/**
		 * OccluderSelectionSetID is the name of the selection set containing all the 
		 * scene meshes used when computing visibility. If set to NULL, no occluders in 
		 * the scene will be used. If both ID and Name are set, ID will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetOccluderSelectionSetName( const char * value ) = 0;

		/**
		 * OccluderSelectionSetID is the name of the selection set containing all the 
		 * scene meshes used when computing visibility. If set to NULL, no occluders in 
		 * the scene will be used. If both ID and Name are set, ID will be used. 
		 */
		virtual	rstring GetOccluderSelectionSetName(  ) = 0;

		/**
		 * If a group of non-visible triangles (connected to visible triangles) has an 
		 * area below the FillNonVisibleAreaThreshold - it will receive the same visibility 
		 * as the neighboring visible triangles. Set to zero to skip filling nonvisible 
		 * regions. 
		 * @param value the new FillNonVisibleAreaThreshold value 
		 */
		virtual	void SetFillNonVisibleAreaThreshold( real value ) = 0;

		/**
		 * If a group of non-visible triangles (connected to visible triangles) has an 
		 * area below the FillNonVisibleAreaThreshold - it will receive the same visibility 
		 * as the neighboring visible triangles. Set to zero to skip filling nonvisible 
		 * regions. 
		 * @return the current FillNonVisibleAreaThreshold value 
		 */
		virtual	real GetFillNonVisibleAreaThreshold(  ) = 0;

		/**
		 * If enabled, will remove all the visible triangles that are not occluding any 
		 * other triangle. It will also remove all non-visible triangles. 
		 * @param value the new RemoveTrianglesNotOccludingOtherTriangles value 
		 */
		virtual	void SetRemoveTrianglesNotOccludingOtherTriangles( bool value ) = 0;

		/**
		 * If enabled, will remove all the visible triangles that are not occluding any 
		 * other triangle. It will also remove all non-visible triangles. 
		 * @return the current RemoveTrianglesThatDontOccludeOtherTriangles flag 
		 */
		virtual	bool GetRemoveTrianglesNotOccludingOtherTriangles(  ) = 0;

		/**
		 * If enabled, will compute the visibility conservatively - meaning that triangles 
		 *  that are visible will be tagged as visible but some non-visible triangles 
		 * might also be tagged as visible. If this is turned off, then it is no longer 
		 * guaranteed that all visible triangles are found - but more non-visible triangles 
		 * will be identified as non-visible. 
		 * @param value the new ConservativeMode value 
		 */
		virtual	void SetConservativeMode( bool value ) = 0;

		/**
		 * If enabled, will compute the visibility conservatively - meaning that triangles 
		 *  that are visible will be tagged as visible but some non-visible triangles 
		 * might also be tagged as visible. If this is turned off, then it is no longer 
		 * guaranteed that all visible triangles are found - but more non-visible triangles 
		 * will be identified as non-visible. 
		 * @return the current ConservativeMode flag 
		 */
		virtual	bool GetConservativeMode(  ) = 0;

		/**
		 * Specifies the renderer type when computing visibility ( SG_COMPUTE_VISIBILITY_MODE_[...] 
		 *  ). 
		 * @param value the new ComputeVisibilityMode value 
		 */
		virtual	void SetComputeVisibilityMode( unsigned int value ) = 0;

		/**
		 * Specifies the renderer type when computing visibility ( SG_COMPUTE_VISIBILITY_MODE_[...] 
		 *  ). 
		 * @return the current ComputeVisibilityMode 
		 */
		virtual	unsigned int GetComputeVisibilityMode(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IVisibilitySettings
	 */
	typedef CountedPointer<IVisibilitySettings> spVisibilitySettings;

	/// Conditional build define for the interface IVisibilitySettings
	#define InterfaceDefined_SimplygonSDK_IVisibilitySettings 1

	class IScene;
	class IMappingImage;
	class IRepairSettings;
	class IReductionSettings;
	class IBoneSettings;
	class INormalCalculationSettings;
	class IMappingImageSettings;
	class IVisibilitySettings;

	/**
	 * IReductionProcessor welds, removes t-junctions, removes double-triangles and 
	 * reduces the trianglecount of the Geometry. It also calculates new normals for 
	 * the reduced and optimized Geometry. 
	 */
	class IReductionProcessor : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IReductionProcessor is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IReductionProcessor",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IReductionProcessor pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IReductionProcessor pointer
		 * @return a pointer to the IReductionProcessor object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IReductionProcessor *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IReductionProcessor") )
				return static_cast<IReductionProcessor*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene object to reduce. Please note that internal refrences might be 
		 *  modified by the processor. The caller should refresh any reference that is 
		 * cached outside the scene after running the processor. 
		 * @param value the scene to reduce 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Set the scene object to reduce. Please note that internal refrences might be 
		 *  modified by the processor. The caller should refresh any reference that is 
		 * cached outside the scene after running the processor. 
		 * @return the current scene 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Get the MappingImage object that is generated if MappingImageSettings::GenerateMappingImage 
		 *  is set. The object is of type IMappingImage. If called with an id, it will 
		 * fetch that specific mapping image if using multi-material output. A new object 
		 * is generated for each call to RunProcessing(). 
		 * @return the generated mappingimage 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * Get the MappingImage object that is generated if MappingImageSettings::GenerateMappingImage 
		 *  is set. The object is of type IMappingImage. If called with an id, it will 
		 * fetch that specific mapping image if using multi-material output. A new object 
		 * is generated for each call to RunProcessing(). 
		 * @param OutputMaterialId the value for the OutputMaterialId 
		 * @return the generated mappingimage 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage( rid OutputMaterialId ) = 0;

		/**
		 * The settings for the repair process. 
		 * @return the repair settings object associated with the processor 
		 */
		virtual	CountedPointer<IRepairSettings> GetRepairSettings(  ) = 0;

		/**
		 * The settings for the reduction process. 
		 * @return the reduction settings object associated with the processor 
		 */
		virtual	CountedPointer<IReductionSettings> GetReductionSettings(  ) = 0;

		/**
		 * The settings for the bone removal process. 
		 * @return the bone settings object associated with the processor 
		 */
		virtual	CountedPointer<IBoneSettings> GetBoneSettings(  ) = 0;

		/**
		 * The settings for the normal repair process. 
		 * @return the normal repair settings object associated with the processor 
		 */
		virtual	CountedPointer<INormalCalculationSettings> GetNormalCalculationSettings(  ) = 0;

		/**
		 * The settings for the image mapping process. 
		 * @return the mappingimage settings object associated with the processor 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * The settings for handling visibility weights. 
		 * @return the visibility settings object associated with the processor 
		 */
		virtual	CountedPointer<IVisibilitySettings> GetVisibilitySettings(  ) = 0;

		/**
		 * Returns the max deviation reached for the last processing 
		 * @return the max deviation of the last processing 
		 */
		virtual	real GetResultDeviation(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IReductionProcessor
	 */
	typedef CountedPointer<IReductionProcessor> spReductionProcessor;

	/// Conditional build define for the interface IReductionProcessor
	#define InterfaceDefined_SimplygonSDK_IReductionProcessor 1


	/**
	 * Remeshing settings for the IRemeshingProcessor class. The remesher creates a 
	 * completely new lightweight mesh from the input geometry, and is intended to 
	 * be viewed from the outside. Here you will set the OnScreenSize (larger numbers 
	 * means a more detailed mesh), merge distance (which fills in smaller gaps) and 
	 * a cutting plane (where all parts of the input mesh on the far side of that plane 
	 * will be culled). 
	 */
	class IRemeshingSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IRemeshingSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IRemeshingSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IRemeshingSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IRemeshingSettings pointer
		 * @return a pointer to the IRemeshingSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IRemeshingSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IRemeshingSettings") )
				return static_cast<IRemeshingSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the on-screen rendering size of the geometry. Allowed values are in the 
		 * range 20 to 4000 pixels. 
		 * @param value is the value OnScreenSize will be set to Value cannot be less 
		 * than 20. Value cannot be greater than 4000. 
		 */
		virtual	void SetOnScreenSize( unsigned int value ) = 0;

		/**
		 * Get the on-screen rendering size of the geometry. Allowed values are in the 
		 * range 40 to 4000 pixels. 
		 * @return the current value of OnScreenSize 
		 */
		virtual	unsigned int GetOnScreenSize(  ) = 0;

		/**
		 * Set the on-screen merge distance in pixels. Smaller cavities will be removed. 
		 * This will be capped to the on-screen size of the remeshing. 
		 * @param value is the value MergeDistance will be set to 
		 */
		virtual	void SetMergeDistance( unsigned int value ) = 0;

		/**
		 * Get the on-screen merge distance in pixels. 
		 * @return the current value of MergeDistance 
		 */
		virtual	unsigned int GetMergeDistance(  ) = 0;

		/**
		 * Set the merge boldness. Defined from 0 to 1, low values will only merge deep 
		 * cavities while high values will merge anything that falls under the merge distance. 
		 * @param value is the value MergeBoldness will be set to 
		 */
		virtual	void SetMergeBoldness( real value ) = 0;

		/**
		 * Get the merge boldness. Defined from 0 to 1, low values will only merge deep 
		 * cavities while high values will merge anything that falls under the merge distance. 
		 * @return the current value of MergeBoldness 
		 */
		virtual	real GetMergeBoldness(  ) = 0;

		/**
		 * The maximum size in pixels of a generated triangle Any value from 5 pixels 
		 * and above is allowed. If set to 0, there will be no restriction. 
		 * @param value is the value MaxTriangleSize will be set to 
		 */
		virtual	void SetMaxTriangleSize( unsigned int value ) = 0;

		/**
		 * The maximum size in pixels of a generated triangle Any value from 5 pixels 
		 * and above is allowed. If set to 0, there will be no restriction. 
		 * @return the current value of MaxTriangleSize 
		 */
		virtual	unsigned int GetMaxTriangleSize(  ) = 0;

		/**
		 * Set the surface transfer mode, options are SG_SURFACETRANSFER_FAST and SG_SURFACETRANSFER_ACCURATE 
		 * @param value is the new surface transfer mode 
		 */
		virtual	void SetSurfaceTransferMode( rid value ) = 0;

		/**
		 * Get the surface transfer mode, options are SG_SURFACETRANSFER_FAST and SG_SURFACETRANSFER_ACCURATE 
		 * @return the current surface transfer mode 
		 */
		virtual	rid GetSurfaceTransferMode(  ) = 0;

		/**
		 * Set to true to cap the mesh with user defined cutting planes 
		 * @param value is the bool UseCuttingPlanes will be set to 
		 */
		virtual	void SetUseCuttingPlanes( bool value ) = 0;

		/**
		 * Get the current value of UseCuttingPlanes 
		 * @return the current value of UseCuttingPlanes 
		 */
		virtual	bool GetUseCuttingPlanes(  ) = 0;

		/**
		 * Set/Get the normal hard angle cutoff (in radians) 
		 * @param value is the value to which HardEdgeAngleInRadians will be set 
		 */
		virtual	void SetHardEdgeAngleInRadians( real value ) = 0;

		/**
		 * Set/Get the normal hard angle cutoff (in radians) 
		 * @return the current value of HardEdgeAngleInRadians 
		 */
		virtual	real GetHardEdgeAngleInRadians(  ) = 0;

		/**
		 * Set the TransferNormals flag. If set, the vertex normals channel will be sampled 
		 *  from the original mesh 
		 * @param value is the value to which TransferNormals will be set 
		 */
		virtual	void SetTransferNormals( bool value ) = 0;

		/**
		 * Get the TransferNormals flag. If set, the vertex normals channel will be sampled 
		 *  from the original mesh 
		 * @return the current value of the TransferNormals bool 
		 */
		virtual	bool GetTransferNormals(  ) = 0;

		/**
		 * Set the TransferColors flag. If set, vertex color channels will be sampled 
		 * from the original mesh 
		 * @param value is the bool to which TransferColors will be set 
		 */
		virtual	void SetTransferColors( bool value ) = 0;

		/**
		 * Get the TransferColors flag. If set, vertex color channels will be sampled 
		 * from the original mesh 
		 * @return the current value of the TransferColors bool 
		 */
		virtual	bool GetTransferColors(  ) = 0;

		/**
		 * Set the UseClippingGeometry flag. If set, the terrain selection set along with 
		 * the terrain empty space override will be used to clip the remeshed geometry, 
		 * like a clipping plane. 
		 * @param value is the bool to which UseClippingGeometry will be set 
		 */
		virtual	void SetUseClippingGeometry( bool value ) = 0;

		/**
		 * Get the the UseClippingGeometry flag. If set, the terrain selection set along 
		 * with the terrain empty space override will be used to clip the remeshed geometry, 
		 * like a clipping plane. 
		 * @return the current value of the UseClippingGeometry bool 
		 */
		virtual	bool GetUseClippingGeometry(  ) = 0;

		/**
		 * Sets the EmptySpaceOverride flag. If on, a coordinate in the area that will 
		 * be considered "outside" can be overridden by the user, allowing the remesher 
		 * to be used for things like interiors of caves and rooms. 
		 * @param value is the value to which UseEmptySpaceOverride will be set 
		 */
		virtual	void SetUseEmptySpaceOverride( bool value ) = 0;

		/**
		 * Gets the EmptySpaceOverride flag. If on, a coordinate in the area that will 
		 * be considered "outside" can be overridden by the user, allowing the remesher 
		 * to be used for things like interiors of caves and rooms. 
		 * @return the current value of UseEmptySpaceOverride 
		 */
		virtual	bool GetUseEmptySpaceOverride(  ) = 0;

		/**
		 * Set the empty space override coordinate. 
		 * @param value_realInputDataPtr is a pointer to a 3-tuple real which will be 
		 * copied and used as a marker designating empty space 
		 */
		virtual	void SetEmptySpaceOverride( const real *value_realInputDataPtr ) = 0;

		/**
		 * Get the empty space override coordinate. 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetEmptySpaceOverride( real *realReturnDataPtr ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to -1, all geometries 
		 * in the scene will be processed. 
		 * @param value the desired SelectionSet id 
		 */
		virtual	void SetProcessSelectionSetID( rid value ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to -1, all geometries 
		 * in the scene will be processed. 
		 * @return the current SelectionSet id 
		 */
		virtual	rid GetProcessSelectionSetID(  ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If SetName is not found, all 
		 * geometries in the scene will be processed. If both ID and Name are set, ID 
		 * will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetProcessSelectionSetName( const char * value ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to NULL, all geometries 
		 * in the scene will be processed. If both ID and Name are set, ID will be used. 
		 * @return the current SelectionSet name 
		 */
		virtual	rstring GetProcessSelectionSetName(  ) = 0;

		/**
		 * The ID of the SelectionSet containing all the cutting planes that should be 
		 * used. If set to -1, all cutting planes are selected. Cutting planes will only 
		 * be used if UseCuttingPlanes is true. 
		 * @param value the ID of the selection set containing the cutting planes 
		 */
		virtual	void SetCuttingPlaneSelectionSetID( rid value ) = 0;

		/**
		 * The ID of the SelectionSet containing all the cutting planes that should be 
		 * used. If set to -1, all cutting planes are selected. Cutting planes will only 
		 * be used if UseCuttingPlanes is true. 
		 * @return the ID of the selection set 
		 */
		virtual	rid GetCuttingPlaneSelectionSetID(  ) = 0;

		/**
		 * The name of the SelectionSet containing all the cutting planes that should 
		 * be used. If SetName is not found, all cutting planes are selected. If both 
		 * ID and Name are set, ID will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetCuttingPlaneSelectionSetName( const char * value ) = 0;

		/**
		 * The name of the SelectionSet containing all the cutting planes that should 
		 * be used. If SetName is not found, all cutting planes are selected. If both 
		 * ID and Name are set, ID will be used. 
		 */
		virtual	rstring GetCuttingPlaneSelectionSetName(  ) = 0;

		/**
		 * The ID of the SelectionSet containing the terrain for clipping. If both ID 
		 * and Name are set, ID will be used. 
		 * @param value the ID of the selection set containing the clipping terrain 
		 */
		virtual	void SetClippingGeometrySelectionSetID( rid value ) = 0;

		/**
		 * The ID of the SelectionSet containing the terrain for clipping. If both ID 
		 * and Name are set, ID will be used. 
		 * @return the ID of the selection set 
		 */
		virtual	rid GetClippingGeometrySelectionSetID(  ) = 0;

		/**
		 * The name of the SelectionSet containing the terrain for clipping. If both ID 
		 * and Name are set, ID will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetClippingGeometrySelectionSetName( const char * value ) = 0;

		/**
		 * The name of the SelectionSet containing the terrain for clipping. If both ID 
		 * and Name are set, ID will be used. 
		 */
		virtual	rstring GetClippingGeometrySelectionSetName(  ) = 0;

		/**
		 * Sets the UseClippingGeometryEmptySpaceOverride flag. If on, a coordinate in 
		 * the area that will be considered "outside" for the clipping geometry can be 
		 * overridden by the user. 
		 * @param value is the value to which UseClippingGeometryEmptySpaceOverride will 
		 * be set 
		 */
		virtual	void SetUseClippingGeometryEmptySpaceOverride( bool value ) = 0;

		/**
		 * Gets the UseClippingGeometryEmptySpaceOverride flag. If on, a coordinate in 
		 * the area that will be considered "outside" for the clipping geometry can be 
		 * overridden by the user. 
		 * @return the current value of UseClippingGeometryEmptySpaceOverride 
		 */
		virtual	bool GetUseClippingGeometryEmptySpaceOverride(  ) = 0;

		/**
		 * Sets the UsePreviewMode flag. If on, an alternate algorithm is used for the 
		 * remeshing which is significantly faster, but supports smaller sizes ( up to 
		 * 1000 OnScreenSize ), does not include any advanced features, and may be less 
		 * precise. 
		 * @param value is the value to which UsePreviewMode will be set 
		 */
		virtual	void SetUsePreviewMode( bool value ) = 0;

		/**
		 * Sets the UsePreviewMode flag. If on, an alternate algorithm is used for the 
		 * remeshing which is significantly faster, but supports smaller sizes ( up to 
		 * 1000 OnScreenSize ), does not include any advanced features, and may be less 
		 * precise. 
		 * @return the current value of UsePreviewMode 
		 */
		virtual	bool GetUsePreviewMode(  ) = 0;

		/**
		 * Set the RespectWindingDirection flag. If true, the remeshing processor will 
		 * not map backfacing triangles 
		 * @param value the desired value of RespectWindingDirection 
		 */
		virtual	void SetRespectWindingDirection( bool value ) = 0;

		/**
		 * Get the RespectWindingDirection flag. If true, the remeshing processor will 
		 * not map backfacing triangles 
		 * @return the current value of RespectWindingDirection 
		 */
		virtual	bool GetRespectWindingDirection(  ) = 0;

		/**
		 * Set the clipping geometry empty space override coordinate. 
		 * @param value_realInputDataPtr is a pointer to a 3-tuple real which will be 
		 * copied and used as a marker designating empty space 
		 */
		virtual	void SetClippingGeometryEmptySpaceOverride( const real *value_realInputDataPtr ) = 0;

		/**
		 * Get the clipping geometry empty space override coordinate. 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetClippingGeometryEmptySpaceOverride( real *realReturnDataPtr ) = 0;

		/**
		 * The name of the scene node in the output Scene containing the remeshed geometry 
		 * @param value the desired OutputSceneNodeName 
		 */
		virtual	void SetOutputSceneNodeName( const char * value ) = 0;

		/**
		 * The name of the scene node in the output Scene containing the remeshed geometry 
		 *  
		 */
		virtual	rstring GetOutputSceneNodeName(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IRemeshingSettings
	 */
	typedef CountedPointer<IRemeshingSettings> spRemeshingSettings;

	/// Conditional build define for the interface IRemeshingSettings
	#define InterfaceDefined_SimplygonSDK_IRemeshingSettings 1


	/**
	 * Remeshing settings for the IRemeshingProcessorV2 class. 
	 */
	class IRemeshingSettingsV2 : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IRemeshingSettingsV2 is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IRemeshingSettingsV2",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IRemeshingSettingsV2 pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IRemeshingSettingsV2 pointer
		 * @return a pointer to the IRemeshingSettingsV2 object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IRemeshingSettingsV2 *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IRemeshingSettingsV2") )
				return static_cast<IRemeshingSettingsV2*>(ptr);
			return NULL;
			}

		/**
		 * Set the on-screen rendering size of the geometry. Allowed values are in the 
		 * range 20 to 10000 pixels. 
		 * @param value is the value OnScreenSize will be set to Value cannot be less 
		 * than 20. Value cannot be greater than 10000. 
		 */
		virtual	void SetOnScreenSize( unsigned int value ) = 0;

		/**
		 * Get the on-screen rendering size of the geometry. 
		 * @return the current value of OnScreenSize 
		 */
		virtual	unsigned int GetOnScreenSize(  ) = 0;

		/**
		 * Set the on-screen merge distance in pixels. Cavities and holes with openings 
		 * smaller than this size will be removed, depending on the boldness setting. 
		 * @param value is the value MergeDistance will be set to Value cannot be less 
		 * than 0. Value cannot be greater than 1000. 
		 */
		virtual	void SetMergeDistance( unsigned int value ) = 0;

		/**
		 * Get the on-screen merge distance in pixels. 
		 * @return the current value of MergeDistance 
		 */
		virtual	unsigned int GetMergeDistance(  ) = 0;

		/**
		 * Set the merge boldness. Defined from 0 to 1, low values will only remove the 
		 * most obvious cavities with the deepest interiors, while larger values will 
		 * fill more aggresively and generally generate less triangles in the output mesh. 
		 * @param value is the value MergeBoldness will be set to Value cannot be less 
		 * than 0.0. Value cannot be greater than 1.0. 
		 */
		virtual	void SetMergeBoldness( real value ) = 0;

		/**
		 * Get the merge boldness. Defined from 0 to 1, low values will only remove the 
		 * most obvious cavities with the deepest interiors, while larger values will 
		 * fill more aggresively and generally generate less triangles in the output mesh. 
		 * @return the current value of MergeBoldness 
		 */
		virtual	real GetMergeBoldness(  ) = 0;

		/**
		 * Set the surface transfer mode, options are SG_SURFACETRANSFER_FAST(SG_MODE_FAST) 
		 * and SG_SURFACETRANSFER_ACCURATE(SG_MODE_ACCURATE) 
		 * @param value is the new surface transfer mode 
		 */
		virtual	void SetSurfaceTransferMode( rid value ) = 0;

		/**
		 * Get the surface transfer mode, options are SG_SURFACETRANSFER_FAST(SG_MODE_FAST) 
		 * and SG_SURFACETRANSFER_ACCURATE(SG_MODE_ACCURATE) 
		 * @return the current surface transfer mode 
		 */
		virtual	rid GetSurfaceTransferMode(  ) = 0;

		/**
		 * Set the mesh generation mode, options are SG_MODE_FAST and SG_MODE_ACCURATE. 
		 * When using fast mode, the output meshes are not guaranteed to be deterministic, 
		 * but will have equivalent quality levels. 
		 * @param value is the new mesh generation mode 
		 */
		virtual	void SetMeshGenerationMode( rid value ) = 0;

		/**
		 * Get the mesh generation mode, options are SG_MODE_FAST and SG_MODE_ACCURATE. 
		 * When using fast mode, the output meshes are not guaranteed to be deterministic, 
		 * but will have equivalent quality levels. 
		 * @return the current mesh generation mode 
		 */
		virtual	rid GetMeshGenerationMode(  ) = 0;

		/**
		 * Set/Get the normal hard angle cutoff (in radians) 
		 * @param value is the value to which HardEdgeAngleInRadians will be set 
		 */
		virtual	void SetHardEdgeAngleInRadians( real value ) = 0;

		/**
		 * Set/Get the normal hard angle cutoff (in radians) 
		 * @return the current value of HardEdgeAngleInRadians 
		 */
		virtual	real GetHardEdgeAngleInRadians(  ) = 0;

		/**
		 * Set the TransferNormals flag. If set, the vertex normals channel will be sampled 
		 *  from the original mesh 
		 * @param value is the value to which TransferNormals will be set 
		 */
		virtual	void SetTransferNormals( bool value ) = 0;

		/**
		 * Get the TransferNormals flag. If set, the vertex normals channel will be sampled 
		 *  from the original mesh 
		 * @return the current value of the TransferNormals bool 
		 */
		virtual	bool GetTransferNormals(  ) = 0;

		/**
		 * Set the TransferColors flag. If set, vertex color channels will be sampled 
		 * from the original mesh 
		 * @param value is the bool to which TransferColors will be set 
		 */
		virtual	void SetTransferColors( bool value ) = 0;

		/**
		 * Get the TransferColors flag. If set, vertex color channels will be sampled 
		 * from the original mesh 
		 * @return the current value of the TransferColors bool 
		 */
		virtual	bool GetTransferColors(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IRemeshingSettingsV2
	 */
	typedef CountedPointer<IRemeshingSettingsV2> spRemeshingSettingsV2;

	/// Conditional build define for the interface IRemeshingSettingsV2
	#define InterfaceDefined_SimplygonSDK_IRemeshingSettingsV2 1

	class IScene;
	class IMappingImage;
	class IRemeshingSettings;
	class IMappingImageSettings;
	class IBoneSettings;
	class IVisibilitySettings;

	/**
	 * IRemeshingProcessor creates a new geometry that is based on the input geometry, 
	 *  but that is topologically based on the size of the input geometry rendered 
	 * at a specific screen size, and allowed pixel error. The remesher will on demand 
	 * also add texture coordinates and a mapping texture to the output remeshed geometry. 
	 * The geometry that is to be remeshed can be defined either using an in-memory 
	 * geometry data object, or using a geometry file placed on disk, the latter being 
	 * the preferred method, if the source geometry is very large as it can be swapped 
	 * in/out of memory as needed. 
	 */
	class IRemeshingProcessor : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IRemeshingProcessor is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IRemeshingProcessor",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IRemeshingProcessor pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IRemeshingProcessor pointer
		 * @return a pointer to the IRemeshingProcessor object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IRemeshingProcessor *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IRemeshingProcessor") )
				return static_cast<IRemeshingProcessor*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene used for remeshing processing. All mesh nodes in the whole scene 
		 * tree will be replaced by one proxy mesh. Please note that internal refrences 
		 * might be modified by the processor. The caller should refresh any reference 
		 * that is cached outside the scene after running the processor. 
		 * @param value the scene to process 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene used for remeshing processing. All mesh nodes in the whole scene 
		 * tree will be replaced by one proxy mesh. Please note that internal refrences 
		 * might be modified by the processor. The caller should refresh any reference 
		 * that is cached outside the scene after running the processor. 
		 * @return the current scene 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Get the MappingImage object that is generated if MappingImageSettings::GenerateMappingImage 
		 *  is set. The object is of type IMappingImage. A new object is generated for 
		 * each call to RunProcessing(). 
		 * @return the current mappingimage 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * Get the MappingImage object that is generated if MappingImageSettings::GenerateMappingImage 
		 *  is set. The object is of type IMappingImage. A new object is generated for 
		 * each call to RunProcessing(). 
		 * @param value the ID of the mapping image 
		 * @return the current mappingimage 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage( rid value ) = 0;

		/**
		 * The settings for the remeshing process. 
		 * @return the remeshing settings object 
		 */
		virtual	CountedPointer<IRemeshingSettings> GetRemeshingSettings(  ) = 0;

		/**
		 * The settings for the image mapping process. 
		 * @return the mapping image settings object 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * The settings for the bone removal process. 
		 * @return the bone settings object 
		 */
		virtual	CountedPointer<IBoneSettings> GetBoneSettings(  ) = 0;

		/**
		 * The settings for handling visibility weights. 
		 * @return the visibility settings object associated with the processor 
		 */
		virtual	CountedPointer<IVisibilitySettings> GetVisibilitySettings(  ) = 0;

		/**
		 * When called after processing, this returns the id of a new selection set in 
		 * the scene which contains the mesh node and geometry created in the remeshing 
		 * processing. 
		 * @return the remeshed geometry selection set id 
		 */
		virtual	rid GetResultSelectionSetId(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RunProcessing RunProcessing() \endlink instead. Runs the remeshing 
		 * of the geometry. Alias of RunProcessing. 
		 */
		virtual	SGDEPRECATED void RemeshGeometry(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IRemeshingProcessor
	 */
	typedef CountedPointer<IRemeshingProcessor> spRemeshingProcessor;

	/// Conditional build define for the interface IRemeshingProcessor
	#define InterfaceDefined_SimplygonSDK_IRemeshingProcessor 1

	class IScene;
	class IMappingImage;
	class IRemeshingSettingsV2;
	class IMappingImageSettings;
	class IBoneSettings;

	/**
	 * IRemeshingProcessorV2 is a newer rewrite of the old IRemeshingProcessor. The 
	 * functionality of the old processor will over time be implemented here and the 
	 * old processor will be replaced. The primary advantages so far are massively 
	 * increased maximum OnScreenSize, much faster processing times for larger sizes, 
	 * more intelligent hole filling algorithms, and optional faster mesh generation 
	 * modes. Memory usage can get pretty extreme at higher sizes depending on input 
	 * mesh, so make sure you have a lot of virtual memory if attempting anything >4000. 
	 *  
	 */
	class IRemeshingProcessorV2 : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IRemeshingProcessorV2 is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IRemeshingProcessorV2",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IRemeshingProcessorV2 pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IRemeshingProcessorV2 pointer
		 * @return a pointer to the IRemeshingProcessorV2 object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IRemeshingProcessorV2 *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IRemeshingProcessorV2") )
				return static_cast<IRemeshingProcessorV2*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene used for remeshing processing. All mesh nodes in the whole scene 
		 * tree will be replaced by one proxy mesh. Please note that internal refrences 
		 * might be modified by the processor. The caller should refresh any reference 
		 * that is cached outside the scene after running the processor. 
		 * @param value the scene to process 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene used for remeshing processing. All mesh nodes in the whole scene 
		 * tree will be replaced by one proxy mesh. Please note that internal refrences 
		 * might be modified by the processor. The caller should refresh any reference 
		 * that is cached outside the scene after running the processor. 
		 * @return the current scene 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Get the MappingImage object that is generated if MappingImageSettings::GenerateMappingImage 
		 *  is set. The object is of type IMappingImage. If called with an id, it will 
		 * fetch that specific mapping image if using multi-material output. A new object 
		 * is generated for each call to RunProcessing(). 
		 * @return the generated mappingimage 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * Get the MappingImage object that is generated if MappingImageSettings::GenerateMappingImage 
		 *  is set. The object is of type IMappingImage. If called with an id, it will 
		 * fetch that specific mapping image if using multi-material output. A new object 
		 * is generated for each call to RunProcessing(). 
		 * @param OutputMaterialId the value for the OutputMaterialId 
		 * @return the generated mappingimage 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage( rid OutputMaterialId ) = 0;

		/**
		 * The settings for the remeshing process. 
		 * @return the remeshing settings object 
		 */
		virtual	CountedPointer<IRemeshingSettingsV2> GetRemeshingSettingsV2(  ) = 0;

		/**
		 * The settings for the image mapping process. 
		 * @return the mapping image settings object 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * The settings for the bone removal process. 
		 * @return the bone settings object 
		 */
		virtual	CountedPointer<IBoneSettings> GetBoneSettings(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IRemeshingProcessorV2
	 */
	typedef CountedPointer<IRemeshingProcessorV2> spRemeshingProcessorV2;

	/// Conditional build define for the interface IRemeshingProcessorV2
	#define InterfaceDefined_SimplygonSDK_IRemeshingProcessorV2 1

	class ISceneNode;
	class IMaterialTable;
	class ISceneBoneTable;
	class ISelectionSetTable;
	class ITextureTable;
	class IMappingImage;
	class IMatrix4x4;
	class IValueArray;
	class IGeometryData;

	/**
	 * IScene is the main keeper of a scene and scene graph in simplygon. The scene 
	 * is self-contained, with materials, textures, etc. to describe a full scene. 
	 *  
	 */
	class IScene : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IScene is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IScene",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IScene pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IScene pointer
		 * @return a pointer to the IScene object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IScene *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IScene") )
				return static_cast<IScene*>(ptr);
			return NULL;
			}

		/**
		 * Get the root node of the scene. 
		 * @return the root node 
		 */
		virtual	CountedPointer<ISceneNode> GetRootNode(  ) = 0;

		/**
		 * Get the material table of the scene 
		 * @return the material table 
		 */
		virtual	CountedPointer<IMaterialTable> GetMaterialTable(  ) = 0;

		/**
		 * Get the bone table of the scene 
		 * @return the bone table 
		 */
		virtual	CountedPointer<ISceneBoneTable> GetBoneTable(  ) = 0;

		/**
		 * Get the selection set table of the scene 
		 * @return the selection set table 
		 */
		virtual	CountedPointer<ISelectionSetTable> GetSelectionSetTable(  ) = 0;

		/**
		 * Get the texture table of the scene 
		 * @return the texture table 
		 */
		virtual	CountedPointer<ITextureTable> GetTextureTable(  ) = 0;

		/**
		 * Get the MappingImage object for the scene. 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * Get the CustomFields array collection, where custom data is stored for the 
		 * scene 
		 * @param selectionSetID is the ID of the selection set containing the nodes to 
		 * remove 
		 * @return an array collection containing all custom fields 
		 */
		virtual	void RemoveSceneNodes( int selectionSetID ) = 0;

		/**
		 * Get the CustomFields array collection, where custom data is stored for the 
		 * scene 
		 * @return an array collection containing all custom fields 
		 */
		virtual	void RemoveSceneNodes(  ) = 0;

		/**
		 * Get the CustomFields array collection, where custom data is stored for the 
		 * scene 
		 * @param nodeType is the type of object to remove from the scene (i.e ISceneMesh) 
		 *  
		 * @return an array collection containing all custom fields 
		 */
		virtual	void RemoveSceneNodes( const char * nodeType ) = 0;

		/**
		 * Clears the whole scene and releases all items in the graph. 
		 */
		virtual	void Clear(  ) = 0;

		/**
		 * Loads a scene from file 
		 * @param path is the path of the scene file 
		 * @return true on success, false if file was not found 
		 */
		virtual	bool LoadFromFile( const char * path ) = 0;

		/**
		 * Saves a scene to file 
		 * @param path is the path to which the scene file will be saved 
		 * @return true on success, false if file could not be written to 
		 */
		virtual	bool SaveToFile( const char * path ) = 0;

		/**
		 * Evaluates the global transformation of the specified node at a specific time. 
		 * EvaluateDefaultGlobalTransformation is used to evaluate the default transformation, 
		 * regardless of node animation. 
		 * @param node is the node to evaluate 
		 * @param global_transform is the transformation matrix that receives the global 
		 * transformation 
		 */
		virtual	void EvaluateDefaultGlobalTransformation( ISceneNode *node , IMatrix4x4 *global_transform ) = 0;

		/**
		 * Show a debug window with the scene graph tree. This debug method returns when 
		 * the window is closed. 
		 */
		virtual	void ShowSceneDebugWindow(  ) = 0;

		/**
		 * Get the inferior (minimum) extent of the scene 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 * @return the minimum extent of the scene 
		 */
		virtual	void GetInf( real *realReturnDataPtr ) = 0;

		/**
		 * Get the superior (maximum) extent of the scene 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 * @return the maximum extent of the scene 
		 */
		virtual	void GetSup( real *realReturnDataPtr ) = 0;

		/**
		 * Get the scene radius. 
		 * @return the radius of the scene 
		 */
		virtual	real GetRadius(  ) = 0;

		/**
		 * Calculates the axis-aligned bounding box of the whole scene. If the node tree 
		 * contains no geometric data, or all data is hidden, the bounding box is not 
		 * calculated, and the method returns false. 
		 * @return true if the calculation succeeded 
		 */
		virtual	bool CalculateExtents(  ) = 0;

		/**
		 * Gets a node defined by a path. Please note that the path must contain names 
		 * for each level to be able to find the specified node. 
		 * @param path the path of the node to get 
		 * @return the node associated with path 
		 */
		virtual	CountedPointer<ISceneNode> GetNodeFromPath( const char * path ) = 0;

		/**
		 * Gets a node defined by an id. 
		 * @param node_id is the id of the node to get 
		 * @return the node associated with the id 
		 */
		virtual	CountedPointer<ISceneNode> GetNodeByGUID( const char * node_id ) = 0;

		/**
		 * Gets a node defined by an id from the specified node tree. 
		 * @param node_id is the id of the node to get 
		 * @param node id is the id of the node to get 
		 * @return the node associated with the id 
		 */
		virtual	CountedPointer<ISceneNode> GetNodeByGUID( const char * node_id , ISceneNode *node ) = 0;

		/**
		 * Adds a custom data field. To be able to add a field, it must have a name that 
		 * does not conflict with the existing custom fields. 
		 * @param base_type is the is the type of the field to be added 
		 * @param name is the name of the new field 
		 * @param tuple_size is the tuple size of the new field 
		 */
		virtual	CountedPointer<IValueArray> AddCustomField( rid base_type , const char * name , unsigned int tuple_size ) = 0;

		/**
		 * Removes a custom data field. 
		 * @param name is the name of the field to be removed 
		 */
		virtual	void RemoveCustomField( const char * name ) = 0;

		/**
		 * Adds/Removes/Gets a custom data field. 
		 * @param name is the name of the field to be fetched 
		 * @return an array containing the field data 
		 */
		virtual	CountedPointer<IValueArray> GetCustomField( const char * name ) = 0;

		/**
		 * Checks if a custom field is present in the scene 
		 * @param name is the name of the field to be checked 
		 * @return true is the field exists, false otherwise 
		 */
		virtual	bool HasCustomField( const char * name ) = 0;

		/**
		 * Removes all bones in the scene that are not linked by any ISceneMesh's geometry 
		 *  data. Note that the removal only removes leaf nodes in the scene. 
		 */
		virtual	void RemoveUnlinkedBones(  ) = 0;

		/**
		 * Will remove mesh nodes that consist of empty geometry. The node transformation 
		 * will be added to its children's transformation. If the root node is a sceneMesh 
		 * with empty geometry, it will be replaced with a generic sceneNode having the 
		 * same transformation. 
		 */
		virtual	void Compact(  ) = 0;

		/**
		 * Appends another scene to this scene. All materials, textures etc. 
		 * @param otherScene is the scene object to merge into current scene 
		 */
		virtual	void Append( IScene *otherScene ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link NewCombinedGeometry NewCombinedGeometry() \endlink instead. Combines 
		 * all the geometries from the scene and returns a copy. 
		 * @return a copy of all geometries combined 
		 */
		virtual	SGDEPRECATED CountedPointer<IGeometryData> GetCombinedGeometry(  ) = 0;

		/**
		 * Combines all the geometries from the scene and returns a copy. 
		 * @param selectionSetID is the ID of the selection set containing node ids to 
		 * combine 
		 */
		virtual	CountedPointer<IGeometryData> GetCombinedGeometry( int selectionSetID ) = 0;

		/**
		 * Combines all the geometries from the selection set and returns a copy. If the 
		 * selection set ID is -1, then all the geometries will be copied. 
		 * @param selectionSetID is the ID of the selection set containing node ids to 
		 * combine 
		 * @return a copy of all geometries combined 
		 */
		virtual	CountedPointer<IGeometryData> NewCombinedGeometry( int selectionSetID ) = 0;

		/**
		 * Combines all the geometries from the scene and returns a copy. 
		 * @return a copy of all geometries combined 
		 */
		virtual	CountedPointer<IGeometryData> NewCombinedGeometry(  ) = 0;

		/**
		 * Selects all nodes of the type specified in the parameter and returns the selection 
		 *  set ID. If no types were found, it returns -1. 
		 * @param nodeTypeName the type of node to select, for example "ISceneMesh" 
		 * @return the selection set ID with the selection 
		 */
		virtual	int SelectNodes( const char * nodeTypeName ) = 0;

		/**
		 * Deep copies the source scene, and all things assigned to it, including all 
		 * nodes, geometry data, materials and tables. 
		 * @param src is source scene to copy 
		 */
		virtual	void DeepCopy( IScene *src ) = 0;

		/**
		 * Creates a new deep copy of the scene, and all things assigned to it, including 
		 * all nodes, geometry data, materials and tables. 
		 * @return a deep copy of the scene object 
		 */
		virtual	CountedPointer<IScene> NewCopy(  ) = 0;

		/**
		 * Traverses the scene meshes in the scene and returns the total triangle count 
		 *  
		 */
		virtual	unsigned int GetTriangleCount(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IScene
	 */
	typedef CountedPointer<IScene> spScene;

	/// Conditional build define for the interface IScene
	#define InterfaceDefined_SimplygonSDK_IScene 1

	class IScene;
	class IMatrix4x4;
	class ISceneMesh;
	class IGeometryData;
	class IScenePlane;

	/**
	 * ISceneNode is the base class for all scene graph nodes in simplygon. It has 
	 * relative transformation to its parent, and functions to add children to the 
	 * node. 
	 */
	class ISceneNode : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISceneNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISceneNode",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISceneNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISceneNode pointer
		 * @return a pointer to the ISceneNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISceneNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISceneNode") )
				return static_cast<ISceneNode*>(ptr);
			return NULL;
			}

		/**
		 * Get the scene object of the node 
		 * @return the scene object 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Get the parent node of this node 
		 * @return the parent node 
		 */
		virtual	CountedPointer<ISceneNode> GetParent(  ) = 0;

		/**
		 * Get the relative transformation of this node 
		 * @return the relative transformation as a 4x4 matrix 
		 */
		virtual	CountedPointer<IMatrix4x4> GetRelativeTransform(  ) = 0;

		/**
		 * Set the OriginalName parameter which can be used to specify the original name 
		 * of a node, for debugging of merging purposes. This parameter is not copied 
		 * or cloned, and is not used internally by Simplygon. 
		 * @param value is a string the name is to be set to 
		 */
		virtual	void SetOriginalName( const char * value ) = 0;

		/**
		 * Get the OriginalName parameter which can be used to specify the original name 
		 * of a node, for debugging of merging purposes. This parameter is not copied 
		 * or cloned, and is not used internally by Simplygon. 
		 * @return the value of the OriginalName string 
		 */
		virtual	rstring GetOriginalName(  ) = 0;

		/**
		 * Get the IsVisible flag. If set, the node is visible. 
		 * @param value is the bool that isVisible is to be set to 
		 */
		virtual	void SetIsVisible( bool value ) = 0;

		/**
		 * Get the IsVisible flag. If set, the node is visible. 
		 * @return the value of the isVisible bool 
		 */
		virtual	bool GetIsVisible(  ) = 0;

		/**
		 * Set the IsFrozen flag. If set, the node will not be modified by processing 
		 * modules. 
		 * @param value is the bool that isFrozen is to be set to 
		 */
		virtual	void SetIsFrozen( bool value ) = 0;

		/**
		 * Get the IsFrozen flag. If set, the node will not be modified by processing 
		 * modules. 
		 * @return the value of the isFrozen bool 
		 */
		virtual	bool GetIsFrozen(  ) = 0;

		/**
		 * Set the IsModified flag. If set, the node is new, copied, or modified by processing 
		 *  modules. This can be useful to track modifications in the scene graph. 
		 * @param value is the bool that IsModified is to be set to 
		 */
		virtual	void SetIsModified( bool value ) = 0;

		/**
		 * Get the IsModified flag. If set, the node is new, copied, or modified by processing 
		 *  modules. This can be useful to track modifications in the scene graph. 
		 * @return the value of the IsModified bool 
		 */
		virtual	bool GetIsModified(  ) = 0;

		/**
		 * Gets the unique id for this node. 
		 * @return the GUID for this node 
		 */
		virtual	rstring GetNodeGUID(  ) = 0;

		/**
		 * Sets the unique id for this node. 
		 * @param value is the node id of type string 
		 */
		virtual	void SetNodeGUID( const char * value ) = 0;

		/**
		 * Get the inferior (minimum) extent of the node 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetInf( real *realReturnDataPtr ) = 0;

		/**
		 * Get the superior (maximum) extent of the node 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetSup( real *realReturnDataPtr ) = 0;

		/**
		 * Get the number of direct children of this node 
		 * @return the number of direct children of this node 
		 */
		virtual	unsigned int GetChildCount(  ) = 0;

		/**
		 * Get a pointer to the i:th child of this node 
		 * @param index the local index of the child (0->GetChildCount()-1) 
		 */
		virtual	CountedPointer<ISceneNode> GetChild( rid index ) = 0;

		/**
		 * Finds the first child that has the name 
		 * @param name the name of the child to look for 
		 */
		virtual	CountedPointer<ISceneNode> FindNamedChild( const char * name ) = 0;

		/**
		 * Adds a child node to this node. If the child is already a child of another 
		 * node, it is removed from this node 
		 * @param child the child object. 
		 */
		virtual	void AddChild( ISceneNode *child ) = 0;

		/**
		 * Creates a SceneMesh node as a child node to this node. 
		 * @param geom is the GeometryData object to associate with this node 
		 */
		virtual	CountedPointer<ISceneMesh> CreateChildMesh( IGeometryData *geom ) = 0;

		/**
		 * Creates a ScenePlane node as a child node to this node. 
		 * @param position_realInputDataPtr is the point on the plane 
		 * @param normal_realInputDataPtr is the normal to the plane 
		 */
		virtual	CountedPointer<IScenePlane> CreateChildPlane( const real *position_realInputDataPtr , const real *normal_realInputDataPtr ) = 0;

		/**
		 * Removes a child from the node. 
		 * @param child the child to be removed 
		 */
		virtual	void RemoveChild( ISceneNode *child ) = 0;

		/**
		 * Removes all children of this node 
		 */
		virtual	void RemoveChildren(  ) = 0;

		/**
		 * Removes this node from its parent. 
		 */
		virtual	void RemoveFromParent(  ) = 0;

		/**
		 * Returns true if the node has the specified child 
		 * @param child the child node to look for 
		 * @return true if child exists in the scene, otherwise false 
		 */
		virtual	bool HasChild( ISceneNode *child ) = 0;

		/**
		 * Deep copies this node tree (this node and all the descendants of this node) 
		 * and returns the copy top node. Please note that the copy is not added to any 
		 * scene, and can be added anywhere. 
		 * @return a new copy of the ISceneNode 
		 */
		virtual	CountedPointer<ISceneNode> NewCopy(  ) = 0;

		/**
		 * Clones this node tree (this node and all the descendants of this node) and 
		 * returns the cloned top node. Please note that the copy is not added to any 
		 * scene, and can be added anywhere. Also note that the clone shares all data 
		 * pointers with the original nodes, such as geometry data objects. 
		 * @return a new clone of the ISceneNode 
		 */
		virtual	CountedPointer<ISceneNode> NewClone(  ) = 0;

		/**
		 * Evaluates the default global transformation of the node, regardless of node 
		 * animation. 
		 * @param global_transform the transformation matrix that receives the global 
		 * transformation 
		 */
		virtual	void EvaluateDefaultGlobalTransformation( IMatrix4x4 *global_transform ) = 0;

		/**
		 * Calculates the axis-aligned bounding box of the node and all its children. 
		 * If the node tree contains no geometric data, or all data is hidden, the bounding 
		 * box is not calcluated, and the method returns false. 
		 * @return true if the calculation succeeded 
		 */
		virtual	bool CalculateExtents(  ) = 0;

		/**
		 * Get the path to this node in the scene. Please note that only paths where each 
		 * node has a name can be used to get to the node using the path. 
		 * @return the path to this node in the scene 
		 */
		virtual	rstring GetPath(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISceneNode
	 */
	typedef CountedPointer<ISceneNode> spSceneNode;

	/// Conditional build define for the interface ISceneNode
	#define InterfaceDefined_SimplygonSDK_ISceneNode 1

	class ISceneNode;

	/**
	 * ISceneNodeCollection holds a number of ISceneNode objects. The objects can be 
	 * looked up by their names (if they have been named through IObject::SetName() 
	 * ). 
	 */
	class ISceneNodeCollection : public IObjectCollection
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISceneNodeCollection is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISceneNodeCollection",type)==0 )
				return true;
			return IObjectCollection::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISceneNodeCollection pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISceneNodeCollection pointer
		 * @return a pointer to the ISceneNodeCollection object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISceneNodeCollection *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISceneNodeCollection") )
				return static_cast<ISceneNodeCollection*>(ptr);
			return NULL;
			}

		/**
		 * Adds an object object to the collection. The handle refers to the object until 
		 * it is removed from the collection again. 
		 * @param obj is the node that is to be added to the collection 
		 * @return the handle of the added node in the collection 
		 */
		virtual	rhandle AddSceneNode( ISceneNode *obj ) = 0;

		/**
		 * Gets the object the handle is referring to. 
		 * @param objhandle is the handle of the node 
		 * @return the node associated with the handle 
		 */
		virtual	CountedPointer<ISceneNode> GetSceneNode( rhandle objhandle ) = 0;

		/**
		 * Removes an object from the collection. Only the first occurrence of the object 
		 * is removed. 
		 * @param pobj is the node to be be removed 
		 */
		virtual	void RemoveSceneNode( ISceneNode *pobj ) = 0;

		/**
		 * Finds a named object. 
		 * @param name is the name of the searched for object 
		 * @return the found node, or NULL if no mathing node is found 
		 */
		virtual	CountedPointer<ISceneNode> FindSceneNode( const char * name ) = 0;

		/**
		 * Returns the object of the next item. 
		 * @param phandle_rhandleInputDataPtr is the handle of a node in the collection 
		 *  
		 * @return the node after phandle 
		 */
		virtual	CountedPointer<ISceneNode> GetNextItemAsSceneNode( rhandle *phandle_rhandleInputDataPtr ) = 0;

		/**
		 * Returns true if the object is in the collection 
		 * @param pobj is an arbitrary node 
		 * @return true if pobj is in the current collection, false otherwise 
		 */
		virtual	bool IsSceneNodeInCollection( ISceneNode *pobj ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISceneNodeCollection
	 */
	typedef CountedPointer<ISceneNodeCollection> spSceneNodeCollection;

	/// Conditional build define for the interface ISceneNodeCollection
	#define InterfaceDefined_SimplygonSDK_ISceneNodeCollection 1

	class IGeometryData;

	/**
	 * ISceneMesh is the mesh node in the simplygon scene graph 
	 */
	class ISceneMesh : public ISceneNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISceneMesh is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISceneMesh",type)==0 )
				return true;
			return ISceneNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISceneMesh pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISceneMesh pointer
		 * @return a pointer to the ISceneMesh object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISceneMesh *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISceneMesh") )
				return static_cast<ISceneMesh*>(ptr);
			return NULL;
			}

		/**
		 * Get the geometry data of the mesh node 
		 * @return the geometrydata associated with the scene mesh 
		 */
		virtual	CountedPointer<IGeometryData> GetGeometry(  ) = 0;

		/**
		 * Set the geometry data of the mesh node 
		 * @param value is the geometry that is to be set 
		 */
		virtual	void SetGeometry( IGeometryData *value ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISceneMesh
	 */
	typedef CountedPointer<ISceneMesh> spSceneMesh;

	/// Conditional build define for the interface ISceneMesh
	#define InterfaceDefined_SimplygonSDK_ISceneMesh 1

	class IRealArray;
	class ISceneNode;

	/**
	 * ISceneLodGroup is a LOD grouping for the scene graph 
	 */
	class ISceneLodGroup : public ISceneNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISceneLodGroup is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISceneLodGroup",type)==0 )
				return true;
			return ISceneNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISceneLodGroup pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISceneLodGroup pointer
		 * @return a pointer to the ISceneLodGroup object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISceneLodGroup *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISceneLodGroup") )
				return static_cast<ISceneLodGroup*>(ptr);
			return NULL;
			}

		/**
		 * Gets the threshold distances associated with the LODs of the LOD group. 
		 * @return the thresholds of the LODs as a realarray 
		 */
		virtual	CountedPointer<IRealArray> GetThresholds(  ) = 0;

		/**
		 * Returns the child node to use for rendering based on the distance parameter 
		 * and the current threshold values. 
		 * @param distance is the distance in units 
		 * @return the LOD with the thresholds that contain distance 
		 */
		virtual	CountedPointer<ISceneNode> GetLodForDistance( real distance ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISceneLodGroup
	 */
	typedef CountedPointer<ISceneLodGroup> spSceneLodGroup;

	/// Conditional build define for the interface ISceneLodGroup
	#define InterfaceDefined_SimplygonSDK_ISceneLodGroup 1

	class IRidArray;

	/**
	 * ISceneBone is the bone node in the simplygon scene graph 
	 */
	class ISceneBone : public ISceneNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISceneBone is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISceneBone",type)==0 )
				return true;
			return ISceneNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISceneBone pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISceneBone pointer
		 * @return a pointer to the ISceneBone object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISceneBone *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISceneBone") )
				return static_cast<ISceneBone*>(ptr);
			return NULL;
			}

		/**
		 * Looks through the hierarchy and fetches the previous BoneNode. If none is found, 
		 *  this is a Root-Bone-Node. -1 means it has no parent, otherwise, the ID is 
		 * the parent ID. 
		 * @return the id of the parent bone, or -1 if no parent exists 
		 */
		virtual	rid GetParentBone(  ) = 0;

		/**
		 * Looks through the sub-tree and collects the IDs of the bones found. 
		 * @param child_bones is the array to which the ids of found child bones will 
		 * be written 
		 */
		virtual	void CollectAllChildBones( IRidArray *child_bones ) = 0;

		/**
		 * Get the BoneId 
		 * @return the id of the current scene bone 
		 */
		virtual	rid GetBoneId(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISceneBone
	 */
	typedef CountedPointer<ISceneBone> spSceneBone;

	/// Conditional build define for the interface ISceneBone
	#define InterfaceDefined_SimplygonSDK_ISceneBone 1

	class ISceneBone;

	/**
	 * ISceneBoneTable keeps information on bones used while rendering. 
	 */
	class ISceneBoneTable : public ITable
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISceneBoneTable is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISceneBoneTable",type)==0 )
				return true;
			return ITable::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISceneBoneTable pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISceneBoneTable pointer
		 * @return a pointer to the ISceneBoneTable object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISceneBoneTable *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISceneBoneTable") )
				return static_cast<ISceneBoneTable*>(ptr);
			return NULL;
			}

		/**
		 * Adds a bone to the table. The returned value is the id of the bone within the 
		 * table. 
		 * @param bone is the scenebone to be added 
		 * @return the id of the added bone within the table 
		 */
		virtual	rid AddBone( ISceneBone *bone ) = 0;

		/**
		 * Finds a bone in the table, using the bone name. If multiple bones have the 
		 * same name, the first will be returned. If the bone was not found, the return 
		 * will be NULL. 
		 * @param name is the string to be searched for 
		 * @return the first matching bone, or NULL if no matches are found 
		 */
		virtual	CountedPointer<ISceneBone> FindBone( const char * name ) = 0;

		/**
		 * Finds the id of a bone in the table, using the bone name. If multiple bones 
		 * have the same name, the first will be returned. If the bone was not found, 
		 * the return will be -1. 
		 * @param name is the string to be searched for 
		 * @return the id of the first matching bone, or -1 if no matches are found 
		 */
		virtual	rid FindBoneId( const char * name ) = 0;

		/**
		 * Returns the i:th bone, using the id of the bone 
		 * @param id is the id of the wanted bone 
		 * @return the bone corresponding to the id 
		 */
		virtual	CountedPointer<ISceneBone> GetBone( rid id ) = 0;

		/**
		 * Removes a bone. If GetBone is called with the bone id, the value returned will 
		 * be NULL. 
		 * @param id is the id of the bone that is to be removed 
		 */
		virtual	void RemoveBone( rid id ) = 0;

		/**
		 * Returns the number of bones in the table 
		 * @return the number of bones in the table 
		 */
		virtual	unsigned int GetBonesCount(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISceneBoneTable
	 */
	typedef CountedPointer<ISceneBoneTable> spSceneBoneTable;

	/// Conditional build define for the interface ISceneBoneTable
	#define InterfaceDefined_SimplygonSDK_ISceneBoneTable 1


	/**
	 * IScenePlane is the plane node in the simplygon scene graph that can be used 
	 * in various processes, for example as a cutting plane in the RemeshingProcessor. 
	 *  
	 */
	class IScenePlane : public ISceneNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IScenePlane is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IScenePlane",type)==0 )
				return true;
			return ISceneNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IScenePlane pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IScenePlane pointer
		 * @return a pointer to the IScenePlane object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IScenePlane *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IScenePlane") )
				return static_cast<IScenePlane*>(ptr);
			return NULL;
			}

		/**
		 * Get the position (translation) of the plane 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetPosition( real *realReturnDataPtr ) = 0;

		/**
		 * Set the position (translation) of the plane 
		 * @param vec_realInputDataPtr the new position 
		 */
		virtual	void SetPosition( const real *vec_realInputDataPtr ) = 0;

		/**
		 * Get the normal of the plane 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 */
		virtual	void GetNormal( real *realReturnDataPtr ) = 0;

		/**
		 * Set the normal of the plane 
		 * @param vec_realInputDataPtr the new normal 
		 */
		virtual	void SetNormal( const real *vec_realInputDataPtr ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IScenePlane
	 */
	typedef CountedPointer<IScenePlane> spScenePlane;

	/// Conditional build define for the interface IScenePlane
	#define InterfaceDefined_SimplygonSDK_IScenePlane 1

	class IRealArray;

	/**
	 * ISceneCamera is the camera node in the Simplygon scene graph. ISceneCamera contains 
	 *  settings that specify how the camera works and it contains a number of views 
	 * for that type of camera. A view is a combination of a CameraPosition and a TargetPosition. 
	 *  The SceneCamera can contain any number of views but every element in TargetPositions 
	 *  has to correspond to a tuple element in CameraPosition. TargetPositions is 
	 * not needed if the camera is set to being omni directional. 
	 */
	class ISceneCamera : public ISceneNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISceneCamera is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISceneCamera",type)==0 )
				return true;
			return ISceneNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISceneCamera pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISceneCamera pointer
		 * @return a pointer to the ISceneCamera object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISceneCamera *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISceneCamera") )
				return static_cast<ISceneCamera*>(ptr);
			return NULL;
			}

		/**
		 * Specifies the camera type. SG_CAMERATYPE_PERSPECTIVE - A regular camera that 
		 * uses the FieldOfView. SG_CAMERATYPE_OMNIDIRECTIONAL - An omni directional camera 
		 * view all directions from its position and ignores the TargetPositions and FieldOfView. 
		 *  SG_CAMERATYPE_ORTHOGRAPHIC - Orthographic cameras render the scene in a direction 
		 *  determined by the vector from CameraPositions to TargetPositions, it uses 
		 * OrthographicCameraPixelSize and ignores the FieldOfView and UseNormalizedCoordinates. 
		 * @param value the desired CameraType 
		 */
		virtual	void SetCameraType( unsigned int value ) = 0;

		/**
		 * Specifies the camera type. SG_CAMERATYPE_PERSPECTIVE - A regular camera that 
		 * uses the FieldOfView. SG_CAMERATYPE_OMNIDIRECTIONAL - An omni directional camera 
		 * view all directions from its position and ignores the TargetPositions and FieldOfView. 
		 *  SG_CAMERATYPE_ORTHOGRAPHIC - Orthographic cameras render the scene in a direction 
		 *  determined by the vector from CameraPositions to TargetPositions, it uses 
		 * OrthographicCameraPixelSize and ignores the FieldOfView and UseNormalizedCoordinates. 
		 * @return the current SG_CAMERATYPE_[...] value. 
		 */
		virtual	unsigned int GetCameraType(  ) = 0;

		/**
		 * CameraPositions is an IRealArray (with tuple size 3) where each tuple is a 
		 * 3d camera position. 
		 * @return the current array of camera positions 
		 */
		virtual	CountedPointer<IRealArray> GetCameraPositions(  ) = 0;

		/**
		 * TargetPositions is an IRealArray (with tuple size 3) where each tuple is a 
		 * 3d camera target position. 
		 * @return the current array of target positions 
		 */
		virtual	CountedPointer<IRealArray> GetTargetPositions(  ) = 0;

		/**
		 * Normalized coordinates means that the coordinates are relative to the scene: 
		 * the origin is in the center of the scene and 1 unit length is the distance 
		 * of the scene radius. This can be used to have cameras in front of or above 
		 * the scene for instance without having to know the coordinates of the scene. 
		 * If set to false, then absolute coordinates will be used instead. 
		 * @return the current UseNormalizedCoordinates flag 
		 */
		virtual	bool GetUseNormalizedCoordinates(  ) = 0;

		/**
		 * Normalized coordinates means that the coordinates are relative to the scene: 
		 * the origin is in the center of the scene and 1 unit length is the distance 
		 * of the scene radius. This can be used to have cameras in front of or above 
		 * the scene for instance without having to know the coordinates of the scene. 
		 * If set to false, then absolute coordinates will be used instead. 
		 * @param value the desired UseNormalizedCoordinates flag 
		 */
		virtual	void SetUseNormalizedCoordinates( bool value ) = 0;

		/**
		 * Specifies the length of the side of a square pixel if the camera type is SG_CAMERATYPE_ORTHOGRAPHIC. 
		 * @return the current OrthographicCameraPixelSize value 
		 */
		virtual	real GetOrthographicCameraPixelSize(  ) = 0;

		/**
		 * Specifies the length of the side of a square pixel if the camera type is SG_CAMERATYPE_ORTHOGRAPHIC. 
		 * @param value the desired OrthographicCameraPixelSize value 
		 */
		virtual	void SetOrthographicCameraPixelSize( real value ) = 0;

		/**
		 * The field of view (in radians) of the camera. 
		 * @return the current FieldOfView value 
		 */
		virtual	real GetFieldOfView(  ) = 0;

		/**
		 * The field of view (in radians) of the camera. 
		 * @param value the desired FieldOfView value 
		 */
		virtual	void SetFieldOfView( real value ) = 0;

		/**
		 * Determines the resolution by specifying the field of view per pixel (both vertically 
		 *  and horizontally) if the camera type is SG_CAMERATYPE_PERSPECTIVE or SG_CAMERATYPE_PERSPECTIVE_OMNIDIRECTIONAL. 
		 *  For instance, if the camera type is SG_CAMERATYPE_PERSPECTIVE and has PI/2 
		 * radians field of view (for the entire camera) and the pixel field of view is 
		 * (PI/2)/512 the camera will have 512x512 pixels. 
		 * @return the current PixelFieldOfView value 
		 */
		virtual	real GetPixelFieldOfView(  ) = 0;

		/**
		 * Determines the resolution by specifying the field of view per pixel (both vertically 
		 *  and horizontally) if the camera type is SG_CAMERATYPE_PERSPECTIVE or SG_CAMERATYPE_PERSPECTIVE_OMNIDIRECTIONAL. 
		 *  For instance, if the camera type is SG_CAMERATYPE_PERSPECTIVE and has PI/2 
		 * radians field of view (for the entire camera) and the pixel field of view is 
		 * (PI/2)/512 the camera will have 512x512 pixels. 
		 * @param value the desired PixelFieldOfView value 
		 */
		virtual	void SetPixelFieldOfView( real value ) = 0;

		/**
		 * Returns the number of views. 
		 * @return the number of views. 
		 */
		virtual	unsigned int GetNumberOfViews(  ) = 0;

		/**
		 * Validates the camera properties. Asserts on invalid ISceneCamera. 
		 * @return true if the ISceneCamera is valid. 
		 */
		virtual	bool ValidateCamera(  ) = 0;

		/**
		 * Creates a set of camera views placed on a custom sphere around the center of 
		 * the scene. 
		 * @param fidelity determines both the resolution and the amount of cameras and 
		 * should be between 0 and 10. 
		 * @param pitch_angle amount of degrees the sphere should be pitch rotated 
		 * @param yaw_angle amount of degrees the sphere should be yaw rotated 
		 * @param coverage_angle 360° would be entire sphere, 180° would be hemisphere, 
		 * 0° would be no sphere. 
		 */
		virtual	void SetCustomSphereCameraPath( int fidelity , real pitch_angle , real yaw_angle , real coverage_angle ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISceneCamera
	 */
	typedef CountedPointer<ISceneCamera> spSceneCamera;

	/// Conditional build define for the interface ISceneCamera
	#define InterfaceDefined_SimplygonSDK_ISceneCamera 1


	/**
	 * ISelectionSet is a set of scene node GUIDs referencing scene nodes in a scene. 
	 *  
	 */
	class ISelectionSet : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISelectionSet is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISelectionSet",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISelectionSet pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISelectionSet pointer
		 * @return a pointer to the ISelectionSet object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISelectionSet *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISelectionSet") )
				return static_cast<ISelectionSet*>(ptr);
			return NULL;
			}

		/**
		 * Adds a node id to the set. 
		 * @param NodeId is the node id that is to be added to the set. 
		 */
		virtual	void AddItem( const char * NodeId ) = 0;

		/**
		 * Removes an node id from the set. 
		 * @param NodeId is the node id that is to be removed from to the set. 
		 */
		virtual	void RemoveItem( const char * NodeId ) = 0;

		/**
		 * Clears the set. 
		 */
		virtual	void Clear(  ) = 0;

		/**
		 * Get the number of items in the set. 
		 * @return the number of items in the set. 
		 */
		virtual	unsigned int GetItemCount(  ) = 0;

		/**
		 * Returns the node id in the i:th place in the set. 
		 * @param index is the index of the node id to get from the set. 
		 * @return the node id at the specified index. 
		 */
		virtual	rstring GetItem( unsigned int index ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISelectionSet
	 */
	typedef CountedPointer<ISelectionSet> spSelectionSet;

	/// Conditional build define for the interface ISelectionSet
	#define InterfaceDefined_SimplygonSDK_ISelectionSet 1

	class ISelectionSet;

	/**
	 * ISelectionSetTable keeps a table of ISelectionSet. 
	 */
	class ISelectionSetTable : public ITable
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISelectionSetTable is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISelectionSetTable",type)==0 )
				return true;
			return ITable::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISelectionSetTable pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISelectionSetTable pointer
		 * @return a pointer to the ISelectionSetTable object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISelectionSetTable *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISelectionSetTable") )
				return static_cast<ISelectionSetTable*>(ptr);
			return NULL;
			}

		/**
		 * Adds a selection set to the table. The returned value is the id of the selection 
		 *  set within the table. 
		 * @param selectionSet is the selection set to be added 
		 * @return the id of the added selection set within the table 
		 */
		virtual	rid AddSelectionSet( ISelectionSet *selectionSet ) = 0;

		/**
		 * Finds a selection set in the table, using the name. If multiple selection sets 
		 * have the same name, the first will be returned. If the selection set was not 
		 * found, the return will be NULL. 
		 * @param name is the string to be searched for 
		 * @return the first matching selection set, or NULL if no matches are found 
		 */
		virtual	CountedPointer<ISelectionSet> FindSelectionSet( const char * name ) = 0;

		/**
		 * Finds the id of a selection set in the table, using the name. If multiple selection 
		 *  sets have the same name, the first will be returned. If the selection set 
		 * was not found, the return will be -1. 
		 * @param name is the string to be searched for 
		 * @return the id of the first matching selection set, or -1 if no matches are 
		 * found 
		 */
		virtual	rid FindSelectionSetId( const char * name ) = 0;

		/**
		 * Returns the i:th selection set, using the id of the selection set. 
		 * @param id is the id of the selection set in the table 
		 * @return the selection set corresponding to the id 
		 */
		virtual	CountedPointer<ISelectionSet> GetSelectionSet( rid id ) = 0;

		/**
		 * Removes a selection set. If GetSelectionSet is called with the selection set 
		 * id, the value returned will be NULL. 
		 * @param id is the id of the selection set that is to be removed 
		 */
		virtual	void RemoveSelectionSet( rid id ) = 0;

		/**
		 * Returns the number of selection sets in the table 
		 * @return the number of selection sets in the table 
		 */
		virtual	unsigned int GetSelectionSetCount(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISelectionSetTable
	 */
	typedef CountedPointer<ISelectionSetTable> spSelectionSetTable;

	/// Conditional build define for the interface ISelectionSetTable
	#define InterfaceDefined_SimplygonSDK_ISelectionSetTable 1

	class IImageData;

	/**
	 * IMaterial keeps information on materials used while rendering. 
	 */
	class ITexture : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ITexture is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ITexture",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ITexture pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ITexture pointer
		 * @return a pointer to the ITexture object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ITexture *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ITexture") )
				return static_cast<ITexture*>(ptr);
			return NULL;
			}

		/**
		 * Set the file path of the texture. 
		 * @param value is the path to the texture file 
		 */
		virtual	void SetFilePath( const char * value ) = 0;

		/**
		 * Get the file path of the texture. 
		 * @return the texture file name 
		 */
		virtual	rstring GetFilePath(  ) = 0;

		/**
		 * Set a named texture image data. If this is defined, it will be used instead 
		 * of the texture file defined in SetFilePath() 
		 * @param value is the image data object containing the texture data 
		 */
		virtual	void SetImageData( IImageData *value ) = 0;

		/**
		 * Get a named texture image data. If this is defined, it will be used instead 
		 * of the texture file defined in SetFilePath() 
		 * @return the texture object 
		 */
		virtual	CountedPointer<IImageData> GetImageData(  ) = 0;

		/**
		 * Changes the prefix path (drive, directory) in the texture, with the new one, 
		 * if the current path matches the current_path_prefix. This method is useful 
		 * when external textures are placed in a specific folder, which needs to be moved 
		 * @param current_path_prefix the current prefix path, either relative or absolute 
		 *  
		 * @param new_path_prefix the new prefix path 
		 */
		virtual	void ChangeTexturePrefixPath( const char * current_path_prefix , const char * new_path_prefix ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ITexture
	 */
	typedef CountedPointer<ITexture> spTexture;

	/// Conditional build define for the interface ITexture
	#define InterfaceDefined_SimplygonSDK_ITexture 1

	class ITexture;

	/**
	 * ITextureTable keeps information on textures used while rendering. 
	 */
	class ITextureTable : public ITable
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ITextureTable is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ITextureTable",type)==0 )
				return true;
			return ITable::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ITextureTable pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ITextureTable pointer
		 * @return a pointer to the ITextureTable object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ITextureTable *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ITextureTable") )
				return static_cast<ITextureTable*>(ptr);
			return NULL;
			}

		/**
		 * Adds a texture to the table. The returned value is the id of the texture within 
		 *  the table. 
		 * @param texture the texture to add into the table 
		 * @return the texture id of the texture in the table 
		 */
		virtual	rid AddTexture( ITexture *texture ) = 0;

		/**
		 * Finds a texture in the table, using the texture name. If multiple textures 
		 * have the same name, the first will be returned. If the texture was not found, 
		 * the return will be NULL. 
		 * @param name the texture name to look for 
		 * @return the texture, if found, NULL otherwise 
		 */
		virtual	CountedPointer<ITexture> FindTexture( const char * name ) = 0;

		/**
		 * Finds a texture in the table, using the texture path. If multiple textures 
		 * have the same path, the first will be returned. If the texture was not found, 
		 * the return will be NULL. 
		 * @param path is the texture path 
		 * @return the texture, if found, NULL otherwise 
		 */
		virtual	CountedPointer<ITexture> FindTextureUsingPath( const char * path ) = 0;

		/**
		 * Finds the id of a texture in the table, using the texture name. If multiple 
		 * textures have the same name, the first will be returned. If the texture was 
		 * not found, the return will be -1. 
		 * @param name the texture name to look for 
		 * @return the texture id if found, -1 otherwise 
		 */
		virtual	rid FindTextureId( const char * name ) = 0;

		/**
		 * Sets a texture in the table. 
		 * @param id the id of the texture in the table 
		 * @param texture the texture to set into the table 
		 */
		virtual	void SetTexture( rid id , ITexture *texture ) = 0;

		/**
		 * Returns the i:th texture, using the id of the texture 
		 * @param id the id of the texture in the table 
		 * @return the the texture in the table 
		 */
		virtual	CountedPointer<ITexture> GetTexture( rid id ) = 0;

		/**
		 * Removes a texture. If GetTexture is called with the texture id, the value returned 
		 *  will be NULL. 
		 * @param id the id of the texture in the table 
		 */
		virtual	void RemoveTexture( rid id ) = 0;

		/**
		 * Returns the number of textures in the table 
		 */
		virtual	unsigned int GetTexturesCount(  ) = 0;

		/**
		 * Changes the prefix path (drive, directory) of all matching texture paths in 
		 * all the textures in the table, with the new one. This method is useful when 
		 * external textures are placed in a specific folder, which needs to be moved 
		 * @param current_path_prefix the current prefix path, either relative or absolute 
		 *  
		 * @param new_path_prefix the new prefix path 
		 */
		virtual	void ChangeTexturePrefixPath( const char * current_path_prefix , const char * new_path_prefix ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ITextureTable
	 */
	typedef CountedPointer<ITextureTable> spTextureTable;

	/// Conditional build define for the interface ITextureTable
	#define InterfaceDefined_SimplygonSDK_ITextureTable 1

	class IImageData;

	/**
	 * Class for loading image data from different file formats. 
	 */
	class IImageDataImporter : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IImageDataImporter is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IImageDataImporter",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IImageDataImporter pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IImageDataImporter pointer
		 * @return a pointer to the IImageDataImporter object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IImageDataImporter *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IImageDataImporter") )
				return static_cast<IImageDataImporter*>(ptr);
			return NULL;
			}

		/**
		 * Set the main import file path. This must always be set. 
		 * @param value the desired main import file path 
		 */
		virtual	void SetImportFilePath( const char * value ) = 0;

		/**
		 * Get the main import file path. This must always be set. 
		 * @return the main import file path 
		 */
		virtual	rstring GetImportFilePath(  ) = 0;

		/**
		 * Set the override file extension. This is specified including the dot, eg ".tga" 
		 * @param value the desired override file extension 
		 */
		virtual	void SetExtensionOverride( const char * value ) = 0;

		/**
		 * Get the override file extension. This is specified including the dot, eg ".tga". 
		 * @return the current override file extension 
		 */
		virtual	rstring GetExtensionOverride(  ) = 0;

		/**
		 * Get the imported image data. 
		 * @return the imported image data 
		 */
		virtual	CountedPointer<IImageData> GetImage(  ) = 0;

		/**
		 * Set CapToPowerOfTwo. If set, the width and height of the loaded image will 
		 * be capped to the closest power of two below the size of the image. 
		 * @param value the desired CapToPowerOfTwo flag 
		 */
		virtual	void SetCapToPowerOfTwo( bool value ) = 0;

		/**
		 * Get CapToPowerOfTwo. If set, the width and height of the loaded image will 
		 * be capped to the closest power of two below the size of the image. 
		 * @return the current CapToPowerOfTwo flag 
		 */
		virtual	bool GetCapToPowerOfTwo(  ) = 0;

		/**
		 * Get MaxWidth. If set to a value above 0, the image that is wider will be rescaled 
		 *  to MaxWidth. 
		 * @return the current MaxWidth 
		 */
		virtual	unsigned int GetMaxWidth(  ) = 0;

		/**
		 * Set/Get MaxWidth. If set to a value above 0, the image that is wider will be 
		 * rescaled to MaxWidth. 
		 * @param value the desired MaxWidth 
		 */
		virtual	void SetMaxWidth( unsigned int value ) = 0;

		/**
		 * Get MaxHeight. If set to a value above 0, the image that is higher will be 
		 * rescaled to MaxHeight. 
		 * @return the current MaxHeight 
		 */
		virtual	unsigned int GetMaxHeight(  ) = 0;

		/**
		 * Set MaxHeight. If set to a value above 0, the image that is higher will be 
		 * rescaled to MaxHeight. 
		 * @param value the desired MaxHeight 
		 */
		virtual	void SetMaxHeight( unsigned int value ) = 0;

		/**
		 * Get ImportOnlyHeader. If set to true the call to RunImport() only gets header 
		 * information from the image, and does not load the actual file into memory. 
		 * MaxHeight, MaxWidth, BitsPerPixel, NumberOfChannels and ImageType will be set 
		 * by call to RunImport(). 
		 * @return the current ImportOnlyHeader value 
		 */
		virtual	bool GetImportOnlyHeader(  ) = 0;

		/**
		 * Set ImportOnlyHeader. If set to true only gets header information from the 
		 * image, and does not load the actual file into memory. 
		 * @param value the desired ImportOnlyHeader value 
		 */
		virtual	void SetImportOnlyHeader( bool value ) = 0;

		/**
		 * After RunImport(), returns the NumberOfChannels (1, 3, 4) in the source image. 
		 * @return the current NumberOfChannels 
		 */
		virtual	unsigned int GetNumberOfChannels(  ) = 0;

		/**
		 * After RunImport(), returns the BitsPerPixel (8bpp-64bpp). If divided by NumberOfChannels, 
		 *  will give the number of bits per channel. 
		 * @return the current BitsPerPixel 
		 */
		virtual	unsigned int GetBitsPerPixel(  ) = 0;

		/**
		 * Get ImageType. Return the type of the image used. The supported formats are 
		 * enumerated in the ImageFileType enumeration. 
		 * @return the current ImageType 
		 */
		virtual	rid GetImageType(  ) = 0;

		/**
		 * Runs the import. Note that all parameters must be setup before importing. 
		 */
		virtual	bool RunImport(  ) = 0;

		/**
		 * Close any open file or stream, release any allocated data. 
		 */
		virtual	void Clear(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IImageDataImporter
	 */
	typedef CountedPointer<IImageDataImporter> spImageDataImporter;

	/// Conditional build define for the interface IImageDataImporter
	#define InterfaceDefined_SimplygonSDK_IImageDataImporter 1


	/**
	 * IMappingImageMeshData stores the mapped meshes and their paths in the original 
	 * scene. This can be used to find a specific triangle on the original mesh in 
	 * the scene from the combined id in the mapping image. 
	 */
	class IMappingImageMeshData : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IMappingImageMeshData is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IMappingImageMeshData",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IMappingImageMeshData pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IMappingImageMeshData pointer
		 * @return a pointer to the IMappingImageMeshData object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IMappingImageMeshData *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IMappingImageMeshData") )
				return static_cast<IMappingImageMeshData*>(ptr);
			return NULL;
			}

		/**
		 * Get the MappedGeometriesCount, the number of mapped geometries 
		 * @return the number of mapped geometries in the mapping image 
		 */
		virtual	unsigned int GetMappedGeometriesCount(  ) = 0;

		/**
		 * Get the Id of the first triangle of the geometry at the specified index. 
		 * @param GeometryIndex the index of the geometry in the list of mapped geometries. 
		 *  
		 * @return the start triangle id of the geometry 
		 */
		virtual	rid GetStartTriangleIdOfGeometry( rid GeometryIndex ) = 0;

		/**
		 * Get the mesh path in the scene of the geometry at the specified index. 
		 * @param GeometryIndex the index of the geometry in the list of mapped geometries. 
		 *  
		 * @return the path in the scene of the mesh that has this geometry 
		 */
		virtual	rstring GetScenePathOfGeometry( rid GeometryIndex ) = 0;

		/**
		 * Get the node guid containing the original geometry. 
		 * @param GeometryIndex is index of the geometry in the list of mapped geometries 
		 *  
		 * @return the guid 
		 */
		virtual	rstring GetNodeGuidOfGeometry( rid GeometryIndex ) = 0;

		/**
		 * Clear the MappingImageMeshData 
		 */
		virtual	void Clear(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IMappingImageMeshData
	 */
	typedef CountedPointer<IMappingImageMeshData> spMappingImageMeshData;

	/// Conditional build define for the interface IMappingImageMeshData
	#define InterfaceDefined_SimplygonSDK_IMappingImageMeshData 1


	/**
	 * IShadingNode is the parent of all nodes in a shading network. 
	 */
	class IShadingNode : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingNode",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingNode pointer
		 * @return a pointer to the IShadingNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingNode") )
				return static_cast<IShadingNode*>(ptr);
			return NULL;
			}

		/**
		 * Get the input count 
		 * @return the input count 
		 */
		virtual	unsigned int GetParameterCount(  ) = 0;

		/**
		 * Get the name of a parameter 
		 * @param parameter_id is the id of the input to fetch the name of. 
		 * @return the name of the parameter 
		 */
		virtual	rstring GetParameterName( rid parameter_id ) = 0;

		/**
		 * Set the default parameter values 
		 * @param parameter_id is the id of the input to be set. 
		 * @param r is the value of red to which the default input will be set. 
		 * @param g is the value of green to which the default input will be set. 
		 * @param b is the value of blue to which the default input will be set. 
		 * @param a is the value of alpha to which the default input will be set. 
		 */
		virtual	void SetDefaultParameter( rid parameter_id , real r , real g , real b , real a ) = 0;

		/**
		 * Get the default parameter values 
		 * @param parameter_id is the id of the input to be fetched. 
		 * @return the Red default parameter value 
		 */
		virtual	real GetDefaultParameterRed( rid parameter_id ) = 0;

		/**
		 * Get the default parameter values 
		 * @param parameter_id is the id of the input to be fetched. 
		 * @return the Green default parameter value 
		 */
		virtual	real GetDefaultParameterGreen( rid parameter_id ) = 0;

		/**
		 * Get the default parameter values 
		 * @param parameter_id is the id of the input to be fetched. 
		 * @return the Blue default parameter value 
		 */
		virtual	real GetDefaultParameterBlue( rid parameter_id ) = 0;

		/**
		 * Get the default parameter values 
		 * @param parameter_id is the id of the input to be fetched. 
		 * @return the Alpha default parameter value 
		 */
		virtual	real GetDefaultParameterAlpha( rid parameter_id ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadingNode
	 */
	typedef CountedPointer<IShadingNode> spShadingNode;

	/// Conditional build define for the interface IShadingNode
	#define InterfaceDefined_SimplygonSDK_IShadingNode 1

	class IShadingNode;

	/**
	 * IFilterNode is the parent of all non-source nodes in a shading network. 
	 */
	class IShadingFilterNode : public IShadingNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingFilterNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingFilterNode",type)==0 )
				return true;
			return IShadingNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingFilterNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingFilterNode pointer
		 * @return a pointer to the IShadingFilterNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingFilterNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingFilterNode") )
				return static_cast<IShadingFilterNode*>(ptr);
			return NULL;
			}

		/**
		 * Set the default node inputs 
		 * @param input_id is the id of the input to be set. 
		 * @param input_node is the node to be set as input 
		 */
		virtual	bool SetInput( rid input_id , IShadingNode *input_node ) = 0;

		/**
		 * Get the default node inputs 
		 * @param input_id is the id of the input to be fetched. 
		 * @return the input node 
		 */
		virtual	CountedPointer<IShadingNode> GetInput( rid input_id ) = 0;

		/**
		 * Get if the parameter is inputable. If true, a different node can be set as 
		 * this parameters value. If false, the parameter can only be set manually and 
		 * not through a node input. 
		 * @param param_id is the id of the parameter to be checked. 
		 * @return true if parameter is inputable, false if it is not 
		 */
		virtual	bool GetParameterIsInputable( rid param_id ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadingFilterNode
	 */
	typedef CountedPointer<IShadingFilterNode> spShadingFilterNode;

	/// Conditional build define for the interface IShadingFilterNode
	#define InterfaceDefined_SimplygonSDK_IShadingFilterNode 1


	/**
	 * ITextureNode describes a texture source node in a shading network. 
	 */
	class IShadingTextureNode : public IShadingNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingTextureNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingTextureNode",type)==0 )
				return true;
			return IShadingNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingTextureNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingTextureNode pointer
		 * @return a pointer to the IShadingTextureNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingTextureNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingTextureNode") )
				return static_cast<IShadingTextureNode*>(ptr);
			return NULL;
			}

		/**
		 * Sets the path of the texture file for casting. If both this and texture image 
		 * is set, the casting uses the texture image. 
		 * @param value is the path the texture will be read from. 
		 */
		virtual	void SetTextureName( const char * value ) = 0;

		/**
		 * Gets the path of the texture file for casting. If both this and texture image 
		 * is set, the casting uses the texture image. 
		 * @return the path the texture will be read from. 
		 */
		virtual	rstring GetTextureName(  ) = 0;

		/**
		 * Set the name of the texture coordinate level. If not set, the material uses 
		 * the texture level (index) instead. The TextureLevelName is required when generating 
		 *  shader data. 
		 * @param value the desired texture level name. 
		 */
		virtual	void SetTextureLevelName( const char * value ) = 0;

		/**
		 * Get the name of the texture coordinate level. If not set, the material uses 
		 * the texture level (index) instead. The TextureLevelName is required when generating 
		 *  shader data. 
		 * @return the current texture level name 
		 */
		virtual	rstring GetTextureLevelName(  ) = 0;

		/**
		 * Set the texture texture coordinate level. The texture level is used during 
		 * casting if the TextureLevelName has not been set. Setting the TextureLevelName 
		 * is still required for generating shader code. 
		 * @param value the desired texture level 
		 */
		virtual	void SetTextureLevel( int value ) = 0;

		/**
		 * Set the texture texture coordinate level. The texture level is used during 
		 * casting if the TextureLevelName has not been set. Setting the TextureLevelName 
		 * is still required for generating shader code. 
		 * @return the current texture level 
		 */
		virtual	int GetTextureLevel(  ) = 0;

		/**
		 * Sets the use of sRGB when decoding the texture into an RGBA value 
		 * @param value is the flag to use sRGB. 
		 */
		virtual	void SetUseSRGB( bool value ) = 0;

		/**
		 * Gets the use of sRGB when decoding the texture into an RGBA value 
		 */
		virtual	bool GetUseSRGB(  ) = 0;

		/**
		 * Sets the TileU value, which is multiplied into the U-coordinate when sampling 
		 * @param value is the new value 
		 */
		virtual	void SetTileU( real value ) = 0;

		/**
		 * Gets the TileU value, which is multiplied into the U-coordinate when sampling 
		 * @return the current value 
		 */
		virtual	real GetTileU(  ) = 0;

		/**
		 * Sets the TileV value, which is multiplied into the U-coordinate when sampling 
		 * @param value is the new value 
		 */
		virtual	void SetTileV( real value ) = 0;

		/**
		 * Gets the TileV value, which is multiplied into the U-coordinate when sampling 
		 * @return the current value 
		 */
		virtual	real GetTileV(  ) = 0;

		/**
		 * Sets the OffsetU value, which is will be added to the U-coordinate when sampling 
		 * @param value is the new value 
		 */
		virtual	void SetOffsetU( real value ) = 0;

		/**
		 * Gets the OffsetU value, which is will be added to the U-coordinate when sampling 
		 * @return the current value 
		 */
		virtual	real GetOffsetU(  ) = 0;

		/**
		 * Sets the OffsetV value, which is will be added to the V-coordinate when sampling 
		 * @param value is the new value 
		 */
		virtual	void SetOffsetV( real value ) = 0;

		/**
		 * Gets the OffsetV value, which is will be added to the U-coordinate when sampling 
		 * @return the current value 
		 */
		virtual	real GetOffsetV(  ) = 0;

		/**
		 * Sets the Texture Wrap Mode, which determines what happens to texture samples 
		 * outside the 0-1 range 
		 * @param value is the new SG_TEXTUREWRAPMODE_[...] value 
		 */
		virtual	void SetTextureWrapMode( int value ) = 0;

		/**
		 * Gets the Texture Wrap Mode, which determines what happens to texture samples 
		 * outside the 0-1 range 
		 * @return the current SG_TEXTUREWRAPMODE_[...] value 
		 */
		virtual	int GetTextureWrapMode(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadingTextureNode
	 */
	typedef CountedPointer<IShadingTextureNode> spShadingTextureNode;

	/// Conditional build define for the interface IShadingTextureNode
	#define InterfaceDefined_SimplygonSDK_IShadingTextureNode 1


	/**
	 * ISwizzlingNode describes a node that can remap the input to the specified output. 
	 *  
	 */
	class IShadingSwizzlingNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingSwizzlingNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingSwizzlingNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingSwizzlingNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingSwizzlingNode pointer
		 * @return a pointer to the IShadingSwizzlingNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingSwizzlingNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingSwizzlingNode") )
				return static_cast<IShadingSwizzlingNode*>(ptr);
			return NULL;
			}

		/**
		 * Set the component index in the input color that Red component will be copied 
		 * from 
		 * @param value the input component index. Allowed values are 0:R 1:G 2:B 3:A 
		 * Value cannot be less than 0. Value cannot be greater than 3. 
		 */
		virtual	void SetRedComponent( unsigned int value ) = 0;

		/**
		 * Get the component index in the input color that Red component will be copied 
		 * from 
		 * @return the input component index. Allowed values are 0:R 1:G 2:B 3:A 
		 */
		virtual	unsigned int GetRedComponent(  ) = 0;

		/**
		 * Set the component index in the input color that Green component will be copied 
		 * from 
		 * @param value the input component index. Allowed values are 0:R 1:G 2:B 3:A 
		 * Value cannot be less than 0. Value cannot be greater than 3. 
		 */
		virtual	void SetGreenComponent( unsigned int value ) = 0;

		/**
		 * Get the component index in the input color that Green component will be copied 
		 * from 
		 * @return the input component index. Allowed values are 0:R 1:G 2:B 3:A 
		 */
		virtual	unsigned int GetGreenComponent(  ) = 0;

		/**
		 * Set the component index in the input color that Blue component will be copied 
		 * from 
		 * @param value the input component index. Allowed values are 0:R 1:G 2:B 3:A 
		 * Value cannot be less than 0. Value cannot be greater than 3. 
		 */
		virtual	void SetBlueComponent( unsigned int value ) = 0;

		/**
		 * Get the component index in the input color that Blue component will be copied 
		 * from 
		 * @return the input component index. Allowed values are 0:R 1:G 2:B 3:A 
		 */
		virtual	unsigned int GetBlueComponent(  ) = 0;

		/**
		 * Set the component index in the input color that Alpha component will be copied 
		 * from 
		 * @param value the input component index. Allowed values are 0:R 1:G 2:B 3:A 
		 * Value cannot be less than 0. Value cannot be greater than 3. 
		 */
		virtual	void SetAlphaComponent( unsigned int value ) = 0;

		/**
		 * Get the component index in the input color that Alpha component will be copied 
		 * from 
		 * @return the input component index. Allowed values are 0:R 1:G 2:B 3:A 
		 */
		virtual	unsigned int GetAlphaComponent(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadingSwizzlingNode
	 */
	typedef CountedPointer<IShadingSwizzlingNode> spShadingSwizzlingNode;

	/// Conditional build define for the interface IShadingSwizzlingNode
	#define InterfaceDefined_SimplygonSDK_IShadingSwizzlingNode 1


	/**
	 * IColorNode describes a single color source node in a shading network 
	 */
	class IShadingColorNode : public IShadingNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingColorNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingColorNode",type)==0 )
				return true;
			return IShadingNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingColorNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingColorNode pointer
		 * @return a pointer to the IShadingColorNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingColorNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingColorNode") )
				return static_cast<IShadingColorNode*>(ptr);
			return NULL;
			}

		/**
		 * Sets the color value 
		 * @param r is the value of the red channel 
		 * @param g is the value of the green channel 
		 * @param b is the value of the blue channel 
		 * @param a is the value of the alpha channel 
		 */
		virtual	void SetColor( real r , real g , real b , real a ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadingColorNode
	 */
	typedef CountedPointer<IShadingColorNode> spShadingColorNode;

	/// Conditional build define for the interface IShadingColorNode
	#define InterfaceDefined_SimplygonSDK_IShadingColorNode 1


	/**
	 * IAddNode describes a single addition shading node in a shading network. Input 
	 * 1 and 2 are added per-component to produce the output. 
	 */
	class IShadingAddNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingAddNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingAddNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingAddNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingAddNode pointer
		 * @return a pointer to the IShadingAddNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingAddNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingAddNode") )
				return static_cast<IShadingAddNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingAddNode
	 */
	typedef CountedPointer<IShadingAddNode> spShadingAddNode;

	/// Conditional build define for the interface IShadingAddNode
	#define InterfaceDefined_SimplygonSDK_IShadingAddNode 1


	/**
	 * ISubtractNode describes a single subtracting shading node in a shading network. 
	 *  Input 2 is subtracted from Input 1 per-component. 
	 */
	class IShadingSubtractNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingSubtractNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingSubtractNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingSubtractNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingSubtractNode pointer
		 * @return a pointer to the IShadingSubtractNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingSubtractNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingSubtractNode") )
				return static_cast<IShadingSubtractNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingSubtractNode
	 */
	typedef CountedPointer<IShadingSubtractNode> spShadingSubtractNode;

	/// Conditional build define for the interface IShadingSubtractNode
	#define InterfaceDefined_SimplygonSDK_IShadingSubtractNode 1


	/**
	 * IMultiplyNode describes a single multiplying shading node in a shading network. 
	 *  Input 1 and Input 2 is multiplied per-component. 
	 */
	class IShadingMultiplyNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingMultiplyNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingMultiplyNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingMultiplyNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingMultiplyNode pointer
		 * @return a pointer to the IShadingMultiplyNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingMultiplyNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingMultiplyNode") )
				return static_cast<IShadingMultiplyNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingMultiplyNode
	 */
	typedef CountedPointer<IShadingMultiplyNode> spShadingMultiplyNode;

	/// Conditional build define for the interface IShadingMultiplyNode
	#define InterfaceDefined_SimplygonSDK_IShadingMultiplyNode 1


	/**
	 * This node describes an exponentiation. The output will be Input 1 to the power 
	 * of Input 2, and it is calculated per-component. 
	 */
	class IShadingPowNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingPowNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingPowNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingPowNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingPowNode pointer
		 * @return a pointer to the IShadingPowNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingPowNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingPowNode") )
				return static_cast<IShadingPowNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingPowNode
	 */
	typedef CountedPointer<IShadingPowNode> spShadingPowNode;

	/// Conditional build define for the interface IShadingPowNode
	#define InterfaceDefined_SimplygonSDK_IShadingPowNode 1


	/**
	 * IDivideNode describes a single division shading node node in a shading network. 
	 *  Input 1 is divided by Input 2 per-component to produce the output. 
	 */
	class IShadingDivideNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingDivideNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingDivideNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingDivideNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingDivideNode pointer
		 * @return a pointer to the IShadingDivideNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingDivideNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingDivideNode") )
				return static_cast<IShadingDivideNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingDivideNode
	 */
	typedef CountedPointer<IShadingDivideNode> spShadingDivideNode;

	/// Conditional build define for the interface IShadingDivideNode
	#define InterfaceDefined_SimplygonSDK_IShadingDivideNode 1


	/**
	 * IClampNode describes a single clamp shading node in a shading network. Input 
	 * 1 is clamped per-component between Input 2 and 3. 
	 */
	class IShadingClampNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingClampNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingClampNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingClampNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingClampNode pointer
		 * @return a pointer to the IShadingClampNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingClampNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingClampNode") )
				return static_cast<IShadingClampNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingClampNode
	 */
	typedef CountedPointer<IShadingClampNode> spShadingClampNode;

	/// Conditional build define for the interface IShadingClampNode
	#define InterfaceDefined_SimplygonSDK_IShadingClampNode 1


	/**
	 * IMaxNode describes a single max shading node in a shading network. The output 
	 * has the per-channel max values of the two inputs. 
	 */
	class IShadingMaxNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingMaxNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingMaxNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingMaxNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingMaxNode pointer
		 * @return a pointer to the IShadingMaxNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingMaxNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingMaxNode") )
				return static_cast<IShadingMaxNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingMaxNode
	 */
	typedef CountedPointer<IShadingMaxNode> spShadingMaxNode;

	/// Conditional build define for the interface IShadingMaxNode
	#define InterfaceDefined_SimplygonSDK_IShadingMaxNode 1


	/**
	 * IMinNode describes a single min shading node in a shading network. The output 
	 * has the per-channel min values of the two inputs. 
	 */
	class IShadingMinNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingMinNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingMinNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingMinNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingMinNode pointer
		 * @return a pointer to the IShadingMinNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingMinNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingMinNode") )
				return static_cast<IShadingMinNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingMinNode
	 */
	typedef CountedPointer<IShadingMinNode> spShadingMinNode;

	/// Conditional build define for the interface IShadingMinNode
	#define InterfaceDefined_SimplygonSDK_IShadingMinNode 1


	/**
	 * IStepNode describes a single step shading node, where the per-component output 
	 * is either 1 or 0 depending on if input0 is larger or smaller than input1. Basically, 
	 *  (Input0 >= Input1) ? 1 : 0 
	 */
	class IShadingStepNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingStepNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingStepNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingStepNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingStepNode pointer
		 * @return a pointer to the IShadingStepNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingStepNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingStepNode") )
				return static_cast<IShadingStepNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingStepNode
	 */
	typedef CountedPointer<IShadingStepNode> spShadingStepNode;

	/// Conditional build define for the interface IShadingStepNode
	#define InterfaceDefined_SimplygonSDK_IShadingStepNode 1


	/**
	 * IShadingEqualNode describes an "is equal to" operator, where the per-component 
	 * output is either 1 or 0 depending on if input0 is equal to input1. Basically, 
	 * (Input0 == Input1) ? 1 : 0 
	 */
	class IShadingEqualNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingEqualNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingEqualNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingEqualNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingEqualNode pointer
		 * @return a pointer to the IShadingEqualNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingEqualNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingEqualNode") )
				return static_cast<IShadingEqualNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingEqualNode
	 */
	typedef CountedPointer<IShadingEqualNode> spShadingEqualNode;

	/// Conditional build define for the interface IShadingEqualNode
	#define InterfaceDefined_SimplygonSDK_IShadingEqualNode 1


	/**
	 * INotEqualNode describes the "is not equal" operator, where the per-component 
	 * output is either 1 or 0 depending on if input0 is equal to input1. Basically, 
	 * (Input0 != Input1) ? 1 : 0 
	 */
	class IShadingNotEqualNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingNotEqualNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingNotEqualNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingNotEqualNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingNotEqualNode pointer
		 * @return a pointer to the IShadingNotEqualNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingNotEqualNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingNotEqualNode") )
				return static_cast<IShadingNotEqualNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingNotEqualNode
	 */
	typedef CountedPointer<IShadingNotEqualNode> spShadingNotEqualNode;

	/// Conditional build define for the interface IShadingNotEqualNode
	#define InterfaceDefined_SimplygonSDK_IShadingNotEqualNode 1


	/**
	 * IVertexColorNode describes a vertex color source node in a shading network. 
	 *  
	 */
	class IShadingVertexColorNode : public IShadingNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingVertexColorNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingVertexColorNode",type)==0 )
				return true;
			return IShadingNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingVertexColorNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingVertexColorNode pointer
		 * @return a pointer to the IShadingVertexColorNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingVertexColorNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingVertexColorNode") )
				return static_cast<IShadingVertexColorNode*>(ptr);
			return NULL;
			}

		/**
		 * DEPRECATED: Use VertexColorSet instead. Sets the vertex color index. This is 
		 * the vertex color field in the geometry which will be used for casting with 
		 * this node. 
		 * @param value vertex color index which will be used. 
		 */
		virtual	void SetVertexColorIndex( rid value ) = 0;

		/**
		 * DEPRECATED: Use VertexColorSet instead. Gets the vertex color index. This is 
		 * the vertex color field in the geometry which will be used for casting with 
		 * this node. 
		 * @return the vertex color index. 
		 */
		virtual	rid GetVertexColorIndex(  ) = 0;

		/**
		 * Sets the vertex color index. This is the vertex color field in the geometry 
		 * which will be used for casting with this node. 
		 * @param value vertex color set which will be used. 
		 */
		virtual	void SetVertexColorSet( const char * value ) = 0;

		/**
		 * Gets the vertex color index. This is the vertex color field in the geometry 
		 * which will be used for casting with this node. 
		 * @return the vertex color set. 
		 */
		virtual	rstring GetVertexColorSet(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadingVertexColorNode
	 */
	typedef CountedPointer<IShadingVertexColorNode> spShadingVertexColorNode;

	/// Conditional build define for the interface IShadingVertexColorNode
	#define InterfaceDefined_SimplygonSDK_IShadingVertexColorNode 1


	/**
	 * IInterpolateNode describes a single interpolating shading node node in a shading 
	 *  network. Input 1 and Input 2 is interpolated per-component using Input 3. Blend 
	 *  values below 0 and over 1 will be clamped. 
	 */
	class IShadingInterpolateNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingInterpolateNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingInterpolateNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingInterpolateNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingInterpolateNode pointer
		 * @return a pointer to the IShadingInterpolateNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingInterpolateNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingInterpolateNode") )
				return static_cast<IShadingInterpolateNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingInterpolateNode
	 */
	typedef CountedPointer<IShadingInterpolateNode> spShadingInterpolateNode;

	/// Conditional build define for the interface IShadingInterpolateNode
	#define InterfaceDefined_SimplygonSDK_IShadingInterpolateNode 1


	/**
	 * ICustomNode describes a custom shading node in a shading network. The output 
	 * is produced based on customizable user specified operations. The custom node 
	 * has a user specified amount of input parameters. Use SetInputCount() to specify 
	 * the number of inputs. The user has to assign and implement event handlers (called 
	 * observers) for the custom node to be able to evaluate to colors. See CustomNodeExample 
	 * and API documentation for further explanations and code samples. 
	 */
	class IShadingCustomNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingCustomNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingCustomNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingCustomNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingCustomNode pointer
		 * @return a pointer to the IShadingCustomNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingCustomNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingCustomNode") )
				return static_cast<IShadingCustomNode*>(ptr);
			return NULL;
			}

		/**
		 * Gets the number of inputs for this custom node 
		 * @return the number of inputs for this custom node 
		 */
		virtual	unsigned int GetInputCount(  ) = 0;

		/**
		 * Sets the number of inputs for this custom node 
		 * @param value the number of inputs for this custom node 
		 */
		virtual	void SetInputCount( unsigned int value ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadingCustomNode
	 */
	typedef CountedPointer<IShadingCustomNode> spShadingCustomNode;

	/// Conditional build define for the interface IShadingCustomNode
	#define InterfaceDefined_SimplygonSDK_IShadingCustomNode 1


	/**
	 * ILayeredBlendNode is a node with an arbitrary number of inputs that can be blended 
	 *  differently. 
	 */
	class IShadingLayeredBlendNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingLayeredBlendNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingLayeredBlendNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingLayeredBlendNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingLayeredBlendNode pointer
		 * @return a pointer to the IShadingLayeredBlendNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingLayeredBlendNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingLayeredBlendNode") )
				return static_cast<IShadingLayeredBlendNode*>(ptr);
			return NULL;
			}

		/**
		 * Gets the number of inputs for this node 
		 * @return the number of inputs for this node 
		 */
		virtual	unsigned int GetInputCount(  ) = 0;

		/**
		 * Sets the number of inputs for this node 
		 * @param value the number of inputs for this node Value cannot be less than 0. 
		 * Value cannot be greater than SG_NUM_SUPPORTED_CUSTOM_NODE_INPUTS. 
		 */
		virtual	void SetInputCount( unsigned int value ) = 0;

		/**
		 * Set the blend type per input connection 
		 * @param _input the input index 
		 * @param _blendType the blend type to use 
		 */
		virtual	void SetPerInputBlendType( int _input , int _blendType ) = 0;

		/**
		 * Get the blend type per input connection 
		 * @param _input the input index 
		 * @return the input blend type 
		 */
		virtual	int GetPerInputBlendType( int _input ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadingLayeredBlendNode
	 */
	typedef CountedPointer<IShadingLayeredBlendNode> spShadingLayeredBlendNode;

	/// Conditional build define for the interface IShadingLayeredBlendNode
	#define InterfaceDefined_SimplygonSDK_IShadingLayeredBlendNode 1


	/**
	 * This node describes an normalization of the x,y,z components of the input. 
	 */
	class IShadingNormalize3Node : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingNormalize3Node is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingNormalize3Node",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingNormalize3Node pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingNormalize3Node pointer
		 * @return a pointer to the IShadingNormalize3Node object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingNormalize3Node *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingNormalize3Node") )
				return static_cast<IShadingNormalize3Node*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingNormalize3Node
	 */
	typedef CountedPointer<IShadingNormalize3Node> spShadingNormalize3Node;

	/// Conditional build define for the interface IShadingNormalize3Node
	#define InterfaceDefined_SimplygonSDK_IShadingNormalize3Node 1


	/**
	 * This node describes a square root calculation. The output will be the per-component 
	 *  square root of input 1 
	 */
	class IShadingSqrtNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingSqrtNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingSqrtNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingSqrtNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingSqrtNode pointer
		 * @return a pointer to the IShadingSqrtNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingSqrtNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingSqrtNode") )
				return static_cast<IShadingSqrtNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingSqrtNode
	 */
	typedef CountedPointer<IShadingSqrtNode> spShadingSqrtNode;

	/// Conditional build define for the interface IShadingSqrtNode
	#define InterfaceDefined_SimplygonSDK_IShadingSqrtNode 1


	/**
	 * This node describes an Dot of the x,y,z components of the input. 
	 */
	class IShadingDot3Node : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingDot3Node is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingDot3Node",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingDot3Node pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingDot3Node pointer
		 * @return a pointer to the IShadingDot3Node object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingDot3Node *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingDot3Node") )
				return static_cast<IShadingDot3Node*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingDot3Node
	 */
	typedef CountedPointer<IShadingDot3Node> spShadingDot3Node;

	/// Conditional build define for the interface IShadingDot3Node
	#define InterfaceDefined_SimplygonSDK_IShadingDot3Node 1


	/**
	 * This node describes the Cross of the x,y,z components of the input. 
	 */
	class IShadingCross3Node : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingCross3Node is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingCross3Node",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingCross3Node pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingCross3Node pointer
		 * @return a pointer to the IShadingCross3Node object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingCross3Node *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingCross3Node") )
				return static_cast<IShadingCross3Node*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingCross3Node
	 */
	typedef CountedPointer<IShadingCross3Node> spShadingCross3Node;

	/// Conditional build define for the interface IShadingCross3Node
	#define InterfaceDefined_SimplygonSDK_IShadingCross3Node 1


	/**
	 * This node describes Cos of the x,y,z,w components of the input. 
	 */
	class IShadingCosNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingCosNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingCosNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingCosNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingCosNode pointer
		 * @return a pointer to the IShadingCosNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingCosNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingCosNode") )
				return static_cast<IShadingCosNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingCosNode
	 */
	typedef CountedPointer<IShadingCosNode> spShadingCosNode;

	/// Conditional build define for the interface IShadingCosNode
	#define InterfaceDefined_SimplygonSDK_IShadingCosNode 1


	/**
	 * This node describes Sin of the x,y,z components of the input. 
	 */
	class IShadingSinNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingSinNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingSinNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingSinNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingSinNode pointer
		 * @return a pointer to the IShadingSinNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingSinNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingSinNode") )
				return static_cast<IShadingSinNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingSinNode
	 */
	typedef CountedPointer<IShadingSinNode> spShadingSinNode;

	/// Conditional build define for the interface IShadingSinNode
	#define InterfaceDefined_SimplygonSDK_IShadingSinNode 1


	/**
	 * IGreaterThanNode describes the "is greater than" operator, where the per-component 
	 *  output is either 1 or 0 depending on if input0 is greater than input1. Basically, 
	 *  (Input0 > Input1) ? 1 : 0 
	 */
	class IShadingGreaterThanNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingGreaterThanNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingGreaterThanNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingGreaterThanNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingGreaterThanNode pointer
		 * @return a pointer to the IShadingGreaterThanNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingGreaterThanNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingGreaterThanNode") )
				return static_cast<IShadingGreaterThanNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingGreaterThanNode
	 */
	typedef CountedPointer<IShadingGreaterThanNode> spShadingGreaterThanNode;

	/// Conditional build define for the interface IShadingGreaterThanNode
	#define InterfaceDefined_SimplygonSDK_IShadingGreaterThanNode 1


	/**
	 * ILessThanNode describes the "is less than" operator, where the per-component 
	 * output is either 1 or 0 depending on if input0 is LessThan to input1. Basically, 
	 * (Input0 < Input1) ? 1 : 0 
	 */
	class IShadingLessThanNode : public IShadingFilterNode
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadingLessThanNode is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadingLessThanNode",type)==0 )
				return true;
			return IShadingFilterNode::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadingLessThanNode pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadingLessThanNode pointer
		 * @return a pointer to the IShadingLessThanNode object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadingLessThanNode *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadingLessThanNode") )
				return static_cast<IShadingLessThanNode*>(ptr);
			return NULL;
			}

	};

	/**
	 * a CounterPointer smart pointer to an IShadingLessThanNode
	 */
	typedef CountedPointer<IShadingLessThanNode> spShadingLessThanNode;

	/// Conditional build define for the interface IShadingLessThanNode
	#define InterfaceDefined_SimplygonSDK_IShadingLessThanNode 1

	class IMaterial;
	class IStringArray;

	/**
	 * Given a material, the shader data class keeps the relevant shader data and is 
	 * able to generate a GLSL/HLSL shader. 
	 */
	class IShaderGenerator : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShaderGenerator is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShaderGenerator",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShaderGenerator pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShaderGenerator pointer
		 * @return a pointer to the IShaderGenerator object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShaderGenerator *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShaderGenerator") )
				return static_cast<IShaderGenerator*>(ptr);
			return NULL;
			}

		/**
		 * Sets the material, for which to create a shader 
		 * @param value is the material object 
		 */
		virtual	void SetMaterial( IMaterial *value ) = 0;

		/**
		 * Gets the material, for which to create a shader 
		 */
		virtual	CountedPointer<IMaterial> GetMaterial(  ) = 0;

		/**
		 * Gets the currently assigned prefix for all global identifiers. 
		 * @return the currently assigned prefix (default="sg_") 
		 */
		virtual	rstring GetIdentifersPrefix(  ) = 0;

		/**
		 * Sets the prefix for all global identifiers. 
		 * @param value is the prefix string 
		 */
		virtual	void SetIdentifersPrefix( const char * value ) = 0;

		/**
		 * Sets the GenerateFullShader flag value. If set, the shader generator will generate 
		 *  a full shader, and not just the material shader tree. If false, the generator 
		 * will define the basic samplers, the per-fragment information and all the sampling 
		 * tree functions, but not the vertex and fragment shaders, that the user will 
		 * have to generate manually. This is useful for custom shader integration. The 
		 * default value is true (create full shader). 
		 * @param value the desired flag value 
		 */
		virtual	void SetGenerateFullShader( bool value ) = 0;

		/**
		 * Gets the GenerateFullShader flag value. If set, the shader generator will generate 
		 *  a full shader, and not just the material shader tree. If false, the generator 
		 * will define the basic samplers, the per-fragment information and all the sampling 
		 * tree functions, but not the vertex and fragment shaders, that the user will 
		 * have to generate manually. This is useful for custom shader integration. The 
		 * default value is true (create full shader). 
		 */
		virtual	bool GetGenerateFullShader(  ) = 0;

		/**
		 * Sets the GenerateTextureSamplerData flag value. If set, the shader generator 
		 * will generate sampler registers and uniforms (HLSL: map_[x] values, state_[x]). 
		 * If not set, the user will need to define them manually. As a help, the code 
		 * is still generated, but commented out. This flag only applies to when the shader 
		 * only generates the shading sampling tree, and is ignored if the GenerateFullShader 
		 * is set. 
		 * @param value the desired flag value 
		 */
		virtual	void SetGenerateTextureSamplerData( bool value ) = 0;

		/**
		 * Gets the GenerateTextureSamplerData flag value. If set, the shader generator 
		 * will generate sampler registers and uniforms (HLSL: map_[x] values, state_[x]). 
		 * If not set, the user will need to define them manually. As a help, the code 
		 * is still generated, but commented out. This flag only applies to when the shader 
		 * only generates the shading sampling tree, and is ignored if the GenerateFullShader 
		 * is set. 
		 */
		virtual	bool GetGenerateTextureSamplerData(  ) = 0;

		/**
		 * Generates a shader, given a material that uses shading node networks. To generate 
		 *  the actual shader code, use GetHLSLCode, GetGLSLVertexCode and GetGLSLFragmentCode 
		 * @return will return true if the shader could be generated from the material 
		 *  
		 */
		virtual	bool GenerateShaderData(  ) = 0;

		/**
		 * Returns a string containing the HLSL shader code. GenerateShader must have 
		 * been called prior to this call 
		 */
		virtual	rstring GetHLSLCode(  ) = 0;

		/**
		 * Returns a string containing the GLSL vertex shader GenerateShader must have 
		 * been called prior to this call. Note: If GenerateFullShader is set to false, 
		 * this code will be empty, as the code generated is only used by the fragment 
		 * shader. 
		 */
		virtual	rstring GetGLSLVertexCode(  ) = 0;

		/**
		 * Returns a string containing the GLSL fragment shader GenerateShader must have 
		 * been called prior to this call 
		 */
		virtual	rstring GetGLSLFragmentCode(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetShaderInputTexturePathsCount GetShaderInputTexturePathsCount() 
		 * \endlink instead. Get an array of the unique texture paths used in the material. 
		 * For the shader to use these textures, they are required to be uploaded to the 
		 * texture channel corresponding to their position in the array. The shader will 
		 * generate these sampler variables with the naming convention "map_i" (i : the 
		 * array index of the texture). GenerateShader must have been called prior to 
		 * this call 
		 * @return the array of unique texture paths 
		 */
		virtual	SGDEPRECATED CountedPointer<IStringArray> GetShaderInputTexturePaths(  ) = 0;

		/**
		 * The number of unique texture paths used in the material. For the shader to 
		 *  use these textures, they are required to be uploaded to the texture channel 
		 * corresponding to their position in the array. The shader will generate these 
		 * sampler variables with the naming convention "map_i" (i : the array index of 
		 * the texture). GenerateShader must have been called prior to this call. Enumerate 
		 * 0 to Count, and use GetShaderInputTexturePath to retrieve a specific path. 
		 * @return the number of unique texture paths 
		 */
		virtual	unsigned int GetShaderInputTexturePathsCount(  ) = 0;

		/**
		 * Get a specific path from the list of unique paths. See notes of GetShaderInputTexturePathsCount 
		 * for more documentation on usage. 
		 * @param index the index of the path to retrieve. 
		 * @return the texture path 
		 */
		virtual	rstring GetShaderInputTexturePath( rid index ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetShaderInputUVSetsCount GetShaderInputUVSetsCount() \endlink instead. 
		 * Get an array of the unique UV sets used in the material. For the shader to 
		 * use these UV sets, they are required to be uploaded as texture coordinates 
		 * corresponding to their position in the array. The shader will generate the 
		 * texcoords variables with the naming convention "tex_i" (i : the array index 
		 * of the UV set). GenerateShader must have been called prior to this call 
		 * @return the array of UV sets (texcoords) 
		 */
		virtual	SGDEPRECATED CountedPointer<IStringArray> GetShaderInputUVSets(  ) = 0;

		/**
		 * The number of unique UV sets used in the material. For the shader to use these 
		 *  UV sets, they are required to be uploaded as texture coordinates corresponding 
		 * to their position in the array. The shader will generate the texcoords variables 
		 * with the naming convention "tex_i" (i : the array index of the UV set). GenerateShader 
		 *  must have been called prior to this call 
		 * @return the number of unique UV sets 
		 */
		virtual	unsigned int GetShaderInputUVSetsCount(  ) = 0;

		/**
		 * Get a specific set from the list of unique sets. See notes of GetShaderInputUVSetsCount 
		 * for more documentation on usage. 
		 * @param index the index of the set to retrieve. 
		 * @return the set name 
		 */
		virtual	rstring GetShaderInputUVSet( rid index ) = 0;

		/**
		 * Returns the index that is set for the specific input UV set. If the UV set 
		 * is not used by the generated shader, the returned value is -1. Note that the 
		 * UV set name must match exactly, including case. 
		 * @param uvset is the uv set name 
		 * @return the index of the uv set, if used, or -1 if not used. 
		 */
		virtual	rid GetShaderInputUVIndex( const char * uvset ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetShaderInputVertexColorsCount GetShaderInputVertexColorsCount() 
		 * \endlink instead. Get an array of the unique vertex colors used in the material. 
		 * For the shader to use these vertex colors, they are required to be uploaded 
		 * as vertex color corresponding to their position in the array. The shader will 
		 * generate the vertex color variables with the naming convention "vertexcolor_i" 
		 * (i : the array index of the vertex color). GenerateShader must have been called 
		 * prior to this call 
		 * @return the array of vertex colors 
		 */
		virtual	SGDEPRECATED CountedPointer<IStringArray> GetShaderInputVertexColors(  ) = 0;

		/**
		 * The number of unique vertex colors used in the material. For the shader to 
		 *  use these vertex colors, they are required to be uploaded as vertex color 
		 * corresponding to their position in the array. The shader will generate the 
		 * vertex color variables with the naming convention "vertexcolor_i" (i : the 
		 * array index of the vertex color). GenerateShader must have been called prior 
		 * to this call 
		 * @return the number of unique color sets 
		 */
		virtual	unsigned int GetShaderInputVertexColorsCount(  ) = 0;

		/**
		 * Get a specific set from the list of unique sets. See notes of GetShaderInputVertexColorsCount 
		 * for more documentation on usage. 
		 * @param index the index of the set to retrieve. 
		 * @return the set name 
		 */
		virtual	rstring GetShaderInputVertexColor( rid index ) = 0;

		/**
		 * Returns the index that is set for the specific input vertex color set. If the 
		 * vertex color set is not used by the generated shader, the returned value is 
		 * -1. Note that the vertex color set name must match exactly, including case. 
		 * @param colorset is the name of the color set 
		 * @return the index of the vertex color set, if used, or -1 if not used. 
		 */
		virtual	rid GetShaderInputVertexColorsIndex( const char * colorset ) = 0;

		/**
		 * Unloads all shader data and resets it. 
		 */
		virtual	void UnloadData(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShaderGenerator
	 */
	typedef CountedPointer<IShaderGenerator> spShaderGenerator;

	/// Conditional build define for the interface IShaderGenerator
	#define InterfaceDefined_SimplygonSDK_IShaderGenerator 1

	class IScene;
	class IMappingImage;
	class IAggregationSettings;
	class IMappingImageSettings;
	class IVisibilitySettings;

	/**
	 * IAggregationProcessor combines all specified geometries in the scene into one 
	 * geometry. All materials are combined and receive a new shared texture atlas. 
	 *  
	 */
	class IAggregationProcessor : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IAggregationProcessor is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IAggregationProcessor",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IAggregationProcessor pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IAggregationProcessor pointer
		 * @return a pointer to the IAggregationProcessor object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IAggregationProcessor *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IAggregationProcessor") )
				return static_cast<IAggregationProcessor*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene used for aggregation processing. Please note that internal refrences 
		 * might be modified by the processor. The caller should refresh any reference 
		 * that is cached outside the scene after running the processor. 
		 * @param value the scene to process 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene used for aggregation processing. Please note that internal refrences 
		 * might be modified by the processor. The caller should refresh any reference 
		 * that is cached outside the scene after running the processor. 
		 * @return the current scene 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Get the MappingImage object that is generated if MappingImageSettings::GenerateMappingImage 
		 *  is set. The object is of type IMappingImage. A new object is generated for 
		 * each call to RunProcessing(). 
		 * @return the current mapping image 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * The settings for the combining process. 
		 * @return the combining settings object 
		 */
		virtual	CountedPointer<IAggregationSettings> GetAggregationSettings(  ) = 0;

		/**
		 * The settings for the image mapping process. 
		 * @return the mapping image settings object 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * The settings for handling visibility weights. 
		 * @return the visibility settings object associated with the processor 
		 */
		virtual	CountedPointer<IVisibilitySettings> GetVisibilitySettings(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IAggregationProcessor
	 */
	typedef CountedPointer<IAggregationProcessor> spAggregationProcessor;

	/// Conditional build define for the interface IAggregationProcessor
	#define InterfaceDefined_SimplygonSDK_IAggregationProcessor 1


	/**
	 * Settings for the IAggregationSettings class. The AggregationProcessor converts 
	 * entire scenes containing multiple draw calls into a single new object with a 
	 * single texture per material channel. The user can set whether or not the AggregationProcessor 
	 * should create new UVs based on the old UVs or create a completely new UV parameterization. 
	 *  
	 */
	class IAggregationSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IAggregationSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IAggregationSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IAggregationSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IAggregationSettings pointer
		 * @return a pointer to the IAggregationSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IAggregationSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IAggregationSettings") )
				return static_cast<IAggregationSettings*>(ptr);
			return NULL;
			}

		/**
		 * Sets whether the new texture coords should be based on the original texture 
		 * coords and the charts only rotated in multiples of 90 degrees, or if the scene 
		 * should have completely new texture coords. 
		 * @param value set to true if the new texture coords should be based on the original 
		 * texture coords. 
		 */
		virtual	void SetBaseAtlasOnOriginalTexCoords( bool value ) = 0;

		/**
		 * Sets whether the new texture coords should be based on the original texture 
		 * coords and the charts only rotated in multiples of 90 degrees, or if the scene 
		 * should have completely new texture coords. 
		 * @return the current flag value 
		 */
		virtual	bool GetBaseAtlasOnOriginalTexCoords(  ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to -1, all geometries 
		 * in the scene will be processed. 
		 * @param value the desired SelectionSet id 
		 */
		virtual	void SetProcessSelectionSetID( rid value ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to -1, all geometries 
		 * in the scene will be processed. 
		 * @return the current SelectionSet id 
		 */
		virtual	rid GetProcessSelectionSetID(  ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If SetName is not found, all 
		 * geometries in the scene will be processed. If both ID and Name are set, ID 
		 * will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetProcessSelectionSetName( const char * value ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to NULL, all geometries 
		 * in the scene will be processed. If both ID and Name are set, ID will be used. 
		 * @return the current SelectionSet name 
		 */
		virtual	rstring GetProcessSelectionSetName(  ) = 0;

		/**
		 * The processor will merge all the geometries in the scene into a single geometry 
		 *  if MergeGeometries is enabled. 
		 * @param value the desired MergeGeometries flag 
		 */
		virtual	void SetMergeGeometries( bool value ) = 0;

		/**
		 * The processor will merge all the geometries in the scene into a single geometry 
		 *  if MergeGeometries is enabled. 
		 * @return value the current MergeGeometries flag 
		 */
		virtual	bool GetMergeGeometries(  ) = 0;

		/**
		 * If true, the processor will merge all the materials in the scene to a new one 
		 * and generate a mapping image using the relevant mapping image settings for 
		 * casting. If false, the materials are kept untouched. 
		 * @param value the desired MergeGeometries flag 
		 */
		virtual	void SetMergeMaterials( bool value ) = 0;

		/**
		 * If true, the processor will merge all the materials in the scene to a new one 
		 * and generate a mapping image using the relevant mapping image settings for 
		 * casting. If false, the materials are kept untouched. 
		 * @return value the current MergeGeometries flag 
		 */
		virtual	bool GetMergeMaterials(  ) = 0;

		/**
		 * Subdivides the geometry based on the tiling texture coords. The cutting frequency 
		 *  "SubdivisionTileSize" is a positive integer multiplier. (i.e. the mesh will 
		 * be cut each time its U or V texcoords crosses the SubdivisionTileSize). The 
		 * output UVs will be in the range [(0,0),(SubdivisionTileSize,SubdivisionTileSize)] 
		 * @param value the desired SubdivideGeometryBasedOnUVTiles flag 
		 */
		virtual	void SetSubdivideGeometryBasedOnUVTiles( bool value ) = 0;

		/**
		 * Subdivides the geometry based on the tiling texture coords. The cutting frequency 
		 *  "SubdivisionTileSize" is a positive integer multiplier. (i.e. the mesh will 
		 * be cut each time its U or V texcoords crosses the SubdivisionTileSize). The 
		 * output UVs will be in the range [(0,0),(SubdivisionTileSize,SubdivisionTileSize)] 
		 * @return value the current SubdivideGeometryBasedOnUVTiles flag 
		 */
		virtual	bool GetSubdivideGeometryBasedOnUVTiles(  ) = 0;

		/**
		 * The cutting frequency used when SubdivideGeometryBasedOnUVTiles is enabled. 
		 * @param value the desired SubdivisionTileSize value 
		 */
		virtual	void SetSubdivisionTileSize( unsigned int value ) = 0;

		/**
		 * The cutting frequency used when SubdivideGeometryBasedOnUVTiles is enabled. 
		 * @return value the current SubdivisionTileSize value 
		 */
		virtual	unsigned int GetSubdivisionTileSize(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IAggregationSettings
	 */
	typedef CountedPointer<IAggregationSettings> spAggregationSettings;

	/// Conditional build define for the interface IAggregationSettings
	#define InterfaceDefined_SimplygonSDK_IAggregationSettings 1

	class IGeometryData;
	class IRealArray;
	class IMaterialTable;
	class ITextureTable;

	/**
	 * Computes new texture coordinates for a geometry with overlapping UV charts. 
	 *  
	 */
	class IChartAggregator : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IChartAggregator is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IChartAggregator",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IChartAggregator pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IChartAggregator pointer
		 * @return a pointer to the IChartAggregator object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IChartAggregator *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IChartAggregator") )
				return static_cast<IChartAggregator*>(ptr);
			return NULL;
			}

		/**
		 * Executes the parameterization of the geometry. 
		 * @param geom is the geometry which is to be parameterized 
		 * @param arr is the array where the result is stored 
		 * @return true if the parameterization succeeds, false otherwise 
		 */
		virtual	bool Parameterize( IGeometryData *geom , IRealArray *arr ) = 0;

		/**
		 * Set the number of input materials used in the original geometry for mapping 
		 * to multiple output materials. This needs to be set before you can set any specific 
		 * in-out material mapping. 0 signifies that no in-out material mapping is used, 
		 * ie. the process will produce one resulting mapping image. 
		 * @param value is the number of outputs 
		 */
		virtual	void SetInputMaterialCount( unsigned int value ) = 0;

		/**
		 * Get the number of input materials set earlier. 0 signifies that no in-out material 
		 *  mapping is used. 
		 * @return the current value of InputMaterialCount 
		 */
		virtual	unsigned int GetInputMaterialCount(  ) = 0;

		/**
		 * Set the number of output mapping images that are to be generated. The triangle 
		 * field OutputMaterialIds also needs to be set, or the material IDs of the original 
		 * geometry needs to be set to a specific output material in this setting object. 
		 * @param value is the number of outputs Value cannot be less than 1. Value cannot 
		 * be greater than SG_MAXIMUM_CHART_AGGREGATOR_OUTPUT_MATERIAL_COUNT. 
		 */
		virtual	void SetOutputMaterialCount( unsigned int value ) = 0;

		/**
		 * Get the number of output mapping images that are to be generated. 
		 * @return the current value of OutputMaterialCount 
		 */
		virtual	unsigned int GetOutputMaterialCount(  ) = 0;

		/**
		 * Set the material mapping for InMaterialId, meaning what material of the generated 
		 * LOD InMaterialId will be baked into. Both InputMaterialCount and OutputMaterialCount 
		 * need to be set for this mapping to work, and all original materials need to 
		 * be mapped to an existing output id. 
		 * @param InMaterialId the input material ID 
		 * @param OutMaterialId the output material ID 
		 */
		virtual	void SetInputOutputMaterialMapping( unsigned int InMaterialId , int OutMaterialId ) = 0;

		/**
		 * Get the previously set material mapping for mat_id. 
		 * @param InMaterialId the input material ID 
		 * @return the currently mapped output material for InMaterialId. -1 means mapping 
		 * is not set. 
		 */
		virtual	int GetInputOutputMaterialMapping( unsigned int InMaterialId ) = 0;

		/**
		 * Set the width of the texture to use. 
		 * @param value the texture width 
		 */
		virtual	void SetTextureWidth( unsigned int value ) = 0;

		/**
		 * Set the width of the texture to use. 
		 * @param id the output material ID 
		 * @param value the texture width 
		 */
		virtual	void SetTextureWidth( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the width of the texture to use. 
		 */
		virtual	unsigned int GetTextureWidth(  ) = 0;

		/**
		 * Get the width of the texture to use. 
		 * @param id the output material ID 
		 * @return the current value of texture width 
		 */
		virtual	unsigned int GetTextureWidth( unsigned int id ) = 0;

		/**
		 * Set the height of the texture to use. 
		 * @param value the texture height 
		 */
		virtual	void SetTextureHeight( unsigned int value ) = 0;

		/**
		 * Set the height of the texture to use. 
		 * @param id the output material ID 
		 * @param value the texture height 
		 */
		virtual	void SetTextureHeight( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the height of the texture to use. 
		 * @return the current value of texture height 
		 */
		virtual	unsigned int GetTextureHeight(  ) = 0;

		/**
		 * Get the height of the texture to use. 
		 * @param id the output material ID 
		 * @return the current value of texture height 
		 */
		virtual	unsigned int GetTextureHeight( unsigned int id ) = 0;

		/**
		 * Set the minimum number of pixels between charts. 
		 * @param value is the value to which GutterSpace will be set 
		 */
		virtual	void SetGutterSpace( unsigned int value ) = 0;

		/**
		 * Set the minimum number of pixels between charts. 
		 * @param id the output material ID 
		 * @param value is the value to which GutterSpace will be set 
		 */
		virtual	void SetGutterSpace( unsigned int id , unsigned int value ) = 0;

		/**
		 * Get the minimum number of pixels between charts. 
		 * @return the current value of GutterSpace 
		 */
		virtual	unsigned int GetGutterSpace(  ) = 0;

		/**
		 * Get the minimum number of pixels between charts. 
		 * @param id the output material ID 
		 * @return the current value of GutterSpace 
		 */
		virtual	unsigned int GetGutterSpace( unsigned int id ) = 0;

		/**
		 * Set the automatic_texture_size flag. If true, then texture sizes will be computed 
		 *  for the reduced mesh depending on its pixel size on screen. 
		 * @param value the desired flag 
		 */
		virtual	void SetUseAutomaticTextureSize( bool value ) = 0;

		/**
		 * Set the automatic_texture_size flag. If true, then texture sizes will be computed 
		 *  for the reduced mesh depending on its pixel size on screen. 
		 * @return the current flag 
		 */
		virtual	bool GetUseAutomaticTextureSize(  ) = 0;

		/**
		 * Set a texture dimension length multiplier for the automatic texture size. 
		 * @param value the desired multiplier 
		 */
		virtual	void SetAutomaticTextureSizeMultiplier( real value ) = 0;

		/**
		 * Get the current automatic texture size multiplier. 
		 * @return the current multiplier 
		 */
		virtual	real GetAutomaticTextureSizeMultiplier(  ) = 0;

		/**
		 * If automatic_texture_size is enabled, then force the texture sizes to be a 
		 * power of 2 
		 * @param value the desired flag 
		 */
		virtual	void SetForcePower2Texture( bool value ) = 0;

		/**
		 * If automatic_texture_size is enabled, then force the texture sizes to be a 
		 * power of 2 
		 * @return the current flag 
		 */
		virtual	bool GetForcePower2Texture(  ) = 0;

		/**
		 * Set which TexCoord level to get charts from during re-parameterization. If 
		 * a TexCoordLevelName is set, it will override the TexCoord level. 
		 * @param value the level/id of the TexCoord to use (i.e. 0-255) 
		 */
		virtual	void SetTexCoordLevel( unsigned int value ) = 0;

		/**
		 * Get which TexCoord level to get charts from during re-parameterization. If 
		 * a TexCoordLevelName is set, it will override the TexCoord level. 
		 * @return the current TexCoord level 
		 */
		virtual	unsigned int GetTexCoordLevel(  ) = 0;

		/**
		 * Set which TexCoord level name to get charts from during re-parameterization. 
		 * @param value the name of the TexCoord to use (i.e. 'TexCood0'-'TexCoord255') 
		 *  
		 */
		virtual	void SetTexCoordLevelName( const char * value ) = 0;

		/**
		 * Get which TexCoord level name to get charts from during re-parameterization. 
		 * @return the current TexCoord level name 
		 */
		virtual	rstring GetTexCoordLevelName(  ) = 0;

		/**
		 * Determines which method to use when aggregating the UVs. SG_CHARTAGGREGATORMODE_TEXTURESIZEPROPORTIONS 
		 *  - Aggregated UV charts will be scaled to keep their relative pixel density 
		 * relative to all other UV charts. The user can still set the size of the output 
		 * texture maps. SG_CHARTAGGREGATORMODE_SURFACEAREA - Aggregated UV charts will 
		 * have their size set based on its actual geometrical size. SG_CHARTAGGREGATORMODE_ORIGINALPIXELDENSITY 
		 *  - The combined atlas will be resized to fit each chart so that all charts 
		 * retain the same amount of pixels as they originally had. This will override 
		 * any manually set texture size. SG_CHARTAGGREGATORMODE_UVSIZEPROPORTIONS - Aggregated 
		 * UV charts will have their size set based on its original UV size, disregarding 
		 * the size of the texture they are used in."); 
		 * @param value the SG_CHARTAGGREGATORMODE_[...] value. 
		 */
		virtual	void SetChartAggregatorMode( unsigned int value ) = 0;

		/**
		 * Determines which method to use when aggregating the UVs. SG_CHARTAGGREGATORMODE_TEXTURESIZEPROPORTIONS 
		 *  - Aggregated UV charts will be scaled to keep their relative pixel density 
		 * relative to all other UV charts. The user can still set the size of the output 
		 * texture maps. SG_CHARTAGGREGATORMODE_SURFACEAREA - Aggregated UV charts will 
		 * have their size set based on its actual geometrical size. SG_CHARTAGGREGATORMODE_ORIGINALPIXELDENSITY 
		 *  - The combined atlas will be resized to fit each chart so that all charts 
		 * retain the same amount of pixels as they originally had. This will override 
		 * any manually set texture size. SG_CHARTAGGREGATORMODE_UVSIZEPROPORTIONS - Aggregated 
		 * UV charts will have their size set based on its original UV size, disregarding 
		 * the size of the texture they are used in."); 
		 * @return the current SG_CHARTAGGREGATORMODE_[...] value. 
		 */
		virtual	unsigned int GetChartAggregatorMode(  ) = 0;

		/**
		 * Set the Separate Overlapping Charts flag. If charts are overlapping in the 
		 * original texture coords, they will be separated. 
		 * @param value the desired flag value 
		 */
		virtual	void SetSeparateOverlappingCharts( bool value ) = 0;

		/**
		 * The scene's material table 
		 * @param value the scene's material table 
		 */
		virtual	void SetMaterialTable( IMaterialTable *value ) = 0;

		/**
		 * The scene's texture table 
		 * @param value the scene's texture table 
		 */
		virtual	void SetTextureTable( ITextureTable *value ) = 0;

		/**
		 * If KeepOriginalChartSizes is enabled: calling GetKeepOriginalChartSizesTextureWidth(int 
		 *  ) and GetKeepOriginalChartSizesTextureHeight() after parameterization will 
		 * return the final texture sizes 
		 * @return the current KeepOriginalChartSizesTextureWidth size 
		 */
		virtual	unsigned int GetKeepOriginalChartSizesTextureWidth(  ) = 0;

		/**
		 * If KeepOriginalChartSizes is enabled: calling GetKeepOriginalChartSizesTextureWidth(int 
		 *  ) and GetKeepOriginalChartSizesTextureHeight() after parameterization will 
		 * return the final texture sizes 
		 * @param OutputID the output id 
		 * @return the current KeepOriginalChartSizesTextureWidth size 
		 */
		virtual	unsigned int GetKeepOriginalChartSizesTextureWidth( unsigned int OutputID ) = 0;

		/**
		 * If KeepOriginalChartSizes is enabled: calling GetKeepOriginalChartSizesTextureWidth() 
		 *  and GetKeepOriginalChartSizesTextureHeight() after parameterization will return 
		 *  the final texture sizes 
		 * @return the current KeepOriginalChartSizesTextureHeight size 
		 */
		virtual	unsigned int GetKeepOriginalChartSizesTextureHeight(  ) = 0;

		/**
		 * If KeepOriginalChartSizes is enabled: calling GetKeepOriginalChartSizesTextureWidth() 
		 *  and GetKeepOriginalChartSizesTextureHeight() after parameterization will return 
		 *  the final texture sizes 
		 * @param OutputID the output id 
		 * @return the current KeepOriginalChartSizesTextureHeight size 
		 */
		virtual	unsigned int GetKeepOriginalChartSizesTextureHeight( unsigned int OutputID ) = 0;

		/**
		 * Set KeepOriginalChartProportionsChannel to an SG_MATERIAL_CHANNEL_[...]. This 
		 * channel determines which texture channel to look at when determining which 
		 * chart sizes to keep. The default value is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @param value the new KeepOriginalChartProportionsChannel value 
		 */
		virtual	void SetOriginalChartProportionsChannel( const char * value ) = 0;

		/**
		 * This channel determines which texture channel to look at when determining which 
		 *  chart proportions to keep. The default value is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @return the current ChartAggregatorOriginalChartProportionsChannel value 
		 */
		virtual	rstring GetOriginalChartProportionsChannel(  ) = 0;

		/**
		 * Get the UseVertexWeights flag to scale the charts according to their vertex 
		 * weights. 
		 * @return flag to using VertexWeights 
		 */
		virtual	bool GetUseVertexWeights(  ) = 0;

		/**
		 * Set the UseVertexWeights flag to scale the charts according to their vertex 
		 * weights. 
		 * @param value the desired flag value 
		 */
		virtual	void SetUseVertexWeights( bool value ) = 0;

		/**
		 * If LockUVRotation is enabled, only the original UV rotation will be used. Otherwise 
		 *  4 rotations are used (90 degree rotations). 
		 * @return the current LockUVRotation 
		 */
		virtual	bool GetLockUVRotation(  ) = 0;

		/**
		 * If LockUVRotation is enabled, only the original UV rotation will be used. Otherwise 
		 *  4 rotations are used (90 degree rotations). 
		 * @param value the new LockUVRotation 
		 */
		virtual	void SetLockUVRotation( bool value ) = 0;

		/**
		 * If ExpandZeroAreaUVs is enabled then triangles that have zero area will be 
		 * slightly expanded 
		 * @return the current ExpandZeroAreaUVs 
		 */
		virtual	bool GetExpandZeroAreaUVs(  ) = 0;

		/**
		 * If ExpandZeroAreaUVs is enabled then triangles that have zero area will be 
		 * slightly expanded 
		 * @param value the new ExpandZeroAreaUVs 
		 */
		virtual	void SetExpandZeroAreaUVs( bool value ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IChartAggregator
	 */
	typedef CountedPointer<IChartAggregator> spChartAggregator;

	/// Conditional build define for the interface IChartAggregator
	#define InterfaceDefined_SimplygonSDK_IChartAggregator 1

	class IScene;
	class ITextureTable;
	class IMaterialTable;
	class IMappingImage;

	/**
	 * IVertexColorBaker 
	 */
	class IVertexColorBaker : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IVertexColorBaker is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IVertexColorBaker",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IVertexColorBaker pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IVertexColorBaker pointer
		 * @return a pointer to the IVertexColorBaker object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IVertexColorBaker *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IVertexColorBaker") )
				return static_cast<IVertexColorBaker*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene used for vertex baking. 
		 * @param value is the scene object 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene used for vertex baking. 
		 * @return the current top scene node 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Set the SourceTextures object. The SourceTextures object contains all textures 
		 * of the the source geometry. 
		 * @param value is the texture table to which SourceTextures will be set 
		 */
		virtual	void SetSourceTextures( ITextureTable *value ) = 0;

		/**
		 * Get the SourceTextures object. The SourceTextures object contains all textures 
		 * of the the source geometry. 
		 * @return the current SourceTextures material table 
		 */
		virtual	CountedPointer<ITextureTable> GetSourceTextures(  ) = 0;

		/**
		 * Set the material table of the scene. 
		 * @param value is the material table containing the source material 
		 */
		virtual	void SetSourceMaterials( IMaterialTable *value ) = 0;

		/**
		 * Get the material table of the scene. 
		 * @return the specified scene material table to use for baking 
		 */
		virtual	CountedPointer<IMaterialTable> GetSourceMaterials(  ) = 0;

		/**
		 * Set the mapping image between original and processed geometry. 
		 * @param _MappingImage is the mapping image object to use for baking. 
		 */
		virtual	void SetMappingImage( IMappingImage *_MappingImage ) = 0;

		/**
		 * Get the mapping image between original and processed geometry. 
		 * @return the mapping image 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * Set the name of the channel to bake to vertex. 
		 * @param value the name of the channel. 
		 */
		virtual	void SetChannelName( const char * value ) = 0;

		/**
		 * Get the name of the channel to bake to vertex. 
		 * @return the name of the channel 
		 */
		virtual	rstring GetInputChannelName(  ) = 0;

		/**
		 * Set the name of the color level field to bake to. If no name is set, it will 
		 * default to "Color0" 
		 * @param value the name of the output color level (i.e. 'Color0'-'Color255') 
		 *  
		 */
		virtual	void SetOutputColorLevelName( const char * value ) = 0;

		/**
		 * Get the name of the color level field to bake to. If no name is set, it will 
		 * default to "Color0" 
		 * @return the name of the color field level. 
		 */
		virtual	rstring GetOutputColorLevelName(  ) = 0;

		/**
		 * Set the color space edge threshold, which will define how many hard color borders 
		 *  there will be. 
		 * @param value is the new threshold. 
		 */
		virtual	void SetColorSpaceEdgeThreshold( real value ) = 0;

		/**
		 * Get the color space edge threshold, which will define how many hard color borders 
		 *  there will be. 
		 * @return the color space edge threshold 
		 */
		virtual	real GetColorSpaceEdgeThreshold(  ) = 0;

		/**
		 * Clears the processing, and resets all internal states. 
		 */
		virtual	void Clear(  ) = 0;

		/**
		 * Run baking to vertex. 
		 */
		virtual	void Bake(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IVertexColorBaker
	 */
	typedef CountedPointer<IVertexColorBaker> spVertexColorBaker;

	/// Conditional build define for the interface IVertexColorBaker
	#define InterfaceDefined_SimplygonSDK_IVertexColorBaker 1

	class IImageData;

	/**
	 * Class for loading image data from different file formats. 
	 */
	class IImageDataExporter : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IImageDataExporter is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IImageDataExporter",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IImageDataExporter pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IImageDataExporter pointer
		 * @return a pointer to the IImageDataExporter object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IImageDataExporter *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IImageDataExporter") )
				return static_cast<IImageDataExporter*>(ptr);
			return NULL;
			}

		/**
		 * Set the main import file path. This must always be set. 
		 * @param value the desired main import file path 
		 */
		virtual	void SetExportFilePath( const char * value ) = 0;

		/**
		 * Get the main import file path. This must always be set. 
		 * @return the main import file path 
		 */
		virtual	rstring GetExportFilePath(  ) = 0;

		/**
		 * Set the imported image data. 
		 * @param value is the image data object to export 
		 * @return the imported image data 
		 */
		virtual	void SetImage( IImageData *value ) = 0;

		/**
		 * Runs the import. Note that all parameters must be setup before importing. 
		 */
		virtual	bool RunExport(  ) = 0;

		/**
		 * Close any open file or stream, release any allocated data. 
		 */
		virtual	void Clear(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IImageDataExporter
	 */
	typedef CountedPointer<IImageDataExporter> spImageDataExporter;

	/// Conditional build define for the interface IImageDataExporter
	#define InterfaceDefined_SimplygonSDK_IImageDataExporter 1

	class IScene;
	class IAmbientOcclusionCasterSettings;

	/**
	 * IAmbientOcclusionCaster generates an ambient occlusion map based on the original 
	 *  geometry, and casts it 
	 */
	class IAmbientOcclusionCaster : public IMaterialCaster
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IAmbientOcclusionCaster is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IAmbientOcclusionCaster",type)==0 )
				return true;
			return IMaterialCaster::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IAmbientOcclusionCaster pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IAmbientOcclusionCaster pointer
		 * @return a pointer to the IAmbientOcclusionCaster object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IAmbientOcclusionCaster *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IAmbientOcclusionCaster") )
				return static_cast<IAmbientOcclusionCaster*>(ptr);
			return NULL;
			}

		/**
		 * Set which material channel to cast. The material channels are defined as SG_MATERIAL_CHANNEL_[ 
		 *  CHANNEL ]. Or a custom channel name. For example, the diffuse channel is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @param ColorType the material channel name 
		 */
		virtual	void SetColorType( const char * ColorType ) = 0;

		/**
		 * Get which material channel to cast. The material channels are defined as SG_MATERIAL_CHANNEL_[ 
		 *  CHANNEL ]. For example, the diffuse channel is SG_MATERIAL_CHANNEL_DIFFUSE. 
		 * @return the current material channel. 
		 */
		virtual	rstring GetColorType(  ) = 0;

		/**
		 * Set the rays per pixel. This determines how many rays are traced per pixel 
		 * (or subpixel) to evaluate the occlusion. 
		 * @param RaysPerPixel the number of rays to use per pixel 
		 */
		virtual	void SetRaysPerPixel( unsigned int RaysPerPixel ) = 0;

		/**
		 * Get the rays per pixel. This determines how many rays are traced per pixel 
		 * (or subpixel) to evaluate the occlusion. 
		 * @return the number of rays 
		 */
		virtual	unsigned int GetRaysPerPixel(  ) = 0;

		/**
		 * Set the occlusion falloff, ie. how far away a surface has to be from another 
		 * surface to generate no occlusion. Only applicable if SimpleOcclusionMode is 
		 * off. 
		 * @param OcclusionFalloff the falloff value 
		 */
		virtual	void SetOcclusionFalloff( real OcclusionFalloff ) = 0;

		/**
		 * Get the occlusion falloff, ie. how far away a surface has to be from another 
		 * surface to generate no occlusion. Only applicable if SimpleOcclusionMode is 
		 * off. 
		 * @return the occlusion falloff 
		 */
		virtual	real GetOcclusionFalloff(  ) = 0;

		/**
		 * Set the occlusion multiplier. This is just a basic intensity scaler. Higher 
		 * is "darker". 
		 * @param OcclusionMultiplier the multiplier 
		 */
		virtual	void SetOcclusionMultiplier( real OcclusionMultiplier ) = 0;

		/**
		 * Get the occlusion multiplier. This is just a basic intensity scaler. Higher 
		 * is "darker". 
		 * @return the occlusion multiplier 
		 */
		virtual	real GetOcclusionMultiplier(  ) = 0;

		/**
		 * Set the SimpleOcclusionMode flag. If set, occlusion will not scale with distance, 
		 *  each ray will only be either fully occluded or not at all. Speeds up processing 
		 *  time. 
		 * @param UseSimpleOcclusionMode the desired flag value 
		 */
		virtual	void SetUseSimpleOcclusionMode( bool UseSimpleOcclusionMode ) = 0;

		/**
		 * Get the SimpleOcclusionMode flag. If set, occlusion will not scale with distance, 
		 *  each ray will only be either fully occluded or not at all. Speeds up processing 
		 *  time. 
		 * @return the current value of UseSimpleOcclusionMode 
		 */
		virtual	bool GetUseSimpleOcclusionMode(  ) = 0;

		/**
		 * Set an additional scene that will be used to compute the ambient occlusion 
		 * on the main scene. 
		 * @param value the additional scene 
		 */
		virtual	void SetAdditionalSourceScene( IScene *value ) = 0;

		/**
		 * Get the ambient occlusion caster settings object 
		 * @return Ambient occlusion caster settings object 
		 */
		virtual	CountedPointer<IAmbientOcclusionCasterSettings> GetAmbientOcclusionCasterSettings(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IAmbientOcclusionCaster
	 */
	typedef CountedPointer<IAmbientOcclusionCaster> spAmbientOcclusionCaster;

	/// Conditional build define for the interface IAmbientOcclusionCaster
	#define InterfaceDefined_SimplygonSDK_IAmbientOcclusionCaster 1


	/**
	 * IAmbientOcclusionCasterSettings manages settings for an ambient occlusion caster 
	 *  
	 */
	class IAmbientOcclusionCasterSettings : public IMaterialCasterSettings
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IAmbientOcclusionCasterSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IAmbientOcclusionCasterSettings",type)==0 )
				return true;
			return IMaterialCasterSettings::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IAmbientOcclusionCasterSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IAmbientOcclusionCasterSettings pointer
		 * @return a pointer to the IAmbientOcclusionCasterSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IAmbientOcclusionCasterSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IAmbientOcclusionCasterSettings") )
				return static_cast<IAmbientOcclusionCasterSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the rays per pixel. This determines how many rays are traced per pixel 
		 * (or subpixel) to evaluate the occlusion. 
		 * @param value the desired number of rays 
		 */
		virtual	void SetRaysPerPixel( unsigned int value ) = 0;

		/**
		 * Get the rays per pixel. This determines how many rays are traced per pixel 
		 * (or subpixel) to evaluate the occlusion. 
		 * @return the number of rays 
		 */
		virtual	unsigned int GetRaysPerPixel(  ) = 0;

		/**
		 * Set the occlusion falloff, ie. how far away a surface has to be from another 
		 * surface to generate no occlusion. Only applicable if SimpleOcclusionMode is 
		 * off. 
		 * @param value the far occlusion falloff 
		 */
		virtual	void SetOcclusionFalloff( real value ) = 0;

		/**
		 * Get the occlusion falloff, ie. how far away a surface has to be from another 
		 * surface to generate no occlusion. Only applicable if SimpleOcclusionMode is 
		 * off. 
		 * @return the occlusion falloff 
		 */
		virtual	real GetOcclusionFalloff(  ) = 0;

		/**
		 * Set the occlusion multiplier. This is just a basic intensity scaler. Higher 
		 * is "darker". 
		 * @param value the occlusion multiplier 
		 */
		virtual	void SetOcclusionMultiplier( real value ) = 0;

		/**
		 * Get the occlusion multiplier. This is just a basic intensity scaler. Higher 
		 * is "darker". 
		 * @return the occlusion multiplier 
		 */
		virtual	real GetOcclusionMultiplier(  ) = 0;

		/**
		 * Set the SimpleOcclusionMode flag. If set, occlusion will not scale with distance, 
		 *  each ray will only be either fully occluded or not at all. Speeds up processing 
		 *  time. 
		 * @param value the flag to which UseSimpleOcclusionMode will be set 
		 */
		virtual	void SetUseSimpleOcclusionMode( bool value ) = 0;

		/**
		 * Get the SimpleOcclusionMode flag. If set, occlusion will not scale with distance, 
		 *  each ray will only be either fully occluded or not at all. Speeds up processing 
		 *  time. 
		 * @return the current value of UseSimpleOcclusionMode 
		 */
		virtual	bool GetUseSimpleOcclusionMode(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IAmbientOcclusionCasterSettings
	 */
	typedef CountedPointer<IAmbientOcclusionCasterSettings> spAmbientOcclusionCasterSettings;

	/// Conditional build define for the interface IAmbientOcclusionCasterSettings
	#define InterfaceDefined_SimplygonSDK_IAmbientOcclusionCasterSettings 1

	class IScene;
	class IGeometryData;
	class IImpostorSettings;
	class IMappingImageSettings;
	class IMappingImage;

	/**
	 * The ImpostorProcessor generates a two-triangle billboard impostor geometry of 
	 * an input geometry from a specific viewing angle, and casts textures and normals 
	 * from the original geometry to the impostor. 
	 */
	class IImpostorProcessor : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IImpostorProcessor is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IImpostorProcessor",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IImpostorProcessor pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IImpostorProcessor pointer
		 * @return a pointer to the IImpostorProcessor object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IImpostorProcessor *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IImpostorProcessor") )
				return static_cast<IImpostorProcessor*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene for the impostor processing. Please note that internal refrences 
		 * might be modified by the processor. The caller should refresh any reference 
		 * that is cached outside the scene after running the processor. 
		 * @param value is the scene 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Set the input geometry used for impostor processing. 
		 * @param value is the input GeometryData 
		 */
		virtual	void SetGeometry( IGeometryData *value ) = 0;

		/**
		 * The settings for the ImpostorProcessor. 
		 * @return the ImpostorProcessor settings object associated with the processor 
		 *  
		 */
		virtual	CountedPointer<IImpostorSettings> GetImpostorSettings(  ) = 0;

		/**
		 * The settings for the the MappingImage. 
		 * @return the MappingImage settings object associated with the processor 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * The MappingImage for the Impostor geometry. 
		 * @return the MappingImage object associated with the processor 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * The Impostor GeometryData. 
		 * @return the Impostor GeometryData object created by the processor 
		 */
		virtual	CountedPointer<IGeometryData> GetImpostorGeometry(  ) = 0;

		/**
		 * Returns the aspect ratio that the impostor from a certain vector will generate, 
		 *  so the mapping setting can be set accordingly. TextureHeight * AspectRatio 
		 * = TextureWidth 
		 */
		virtual	real CalculateImpostorAspectRatio(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IImpostorProcessor
	 */
	typedef CountedPointer<IImpostorProcessor> spImpostorProcessor;

	/// Conditional build define for the interface IImpostorProcessor
	#define InterfaceDefined_SimplygonSDK_IImpostorProcessor 1


	/**
	 * Settings for the ImpostorProccessor 
	 */
	class IImpostorSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IImpostorSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IImpostorSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IImpostorSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IImpostorSettings pointer
		 * @return a pointer to the IImpostorSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IImpostorSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IImpostorSettings") )
				return static_cast<IImpostorSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the UseTightFitting flag. If set, the impostor will exactly conform to 
		 * the bounds of the geometry. If false, its dimensions will always be diameter*diameter 
		 * @param value is the value UseTightFitting will be set to 
		 */
		virtual	void SetUseTightFitting( bool value ) = 0;

		/**
		 * Get the UseTightFitting flag. If set, the impostor will exactly conform to 
		 * the bounds of the geometry. If false, its dimensions will always be diameter*diameter 
		 * @return the current value of UseTightFitting 
		 */
		virtual	bool GetUseTightFitting(  ) = 0;

		/**
		 * Set the depth offset. This determines where the impostor will end up in depth. 
		 * 0 is "center", 1 is "front" and -1 is "back" 
		 * @param value is the value OnScreenSize will be set to 
		 */
		virtual	void SetTightFittingDepthOffset( real value ) = 0;

		/**
		 * Get the depth offset. This determines where the impostor will end up in depth. 
		 * 0 is "center", 1 is "front" and -1 is "back" 
		 * @return the current value of OnScreenSize 
		 */
		virtual	real GetTightFittingDepthOffset(  ) = 0;

		/**
		 * Set the TwoSided flag. If set, the impostor will contain both front and back 
		 * facing surfaces. 
		 * @param value the new TwoSided value 
		 */
		virtual	void SetTwoSided( bool value ) = 0;

		/**
		 * Set the TwoSided flag. If set, the impostor will contain both front and back 
		 * facing surfaces. 
		 * @return the current value of TwoSided 
		 */
		virtual	bool GetTwoSided(  ) = 0;

		/**
		 * Set the view direction. This vector determines how the resulting impostor geometry 
		 *  will be oriented. The billboard will be perpendicular to this vector. 
		 * @param value_realInputDataPtr is the new view vector 
		 */
		virtual	void SetViewDirection( const real *value_realInputDataPtr ) = 0;

		/**
		 * Get the view direction. This vector determines how the resulting impostor geometry 
		 *  will be oriented. The billboard will be perpendicular to this vector. 
		 * @param realReturnDataPtr pointer to a user-provided data area to receive the 
		 * return value. Note! Needs to be at least 3 elements in size 
		 * @return the current view vector 
		 */
		virtual	void GetViewDirection( real *realReturnDataPtr ) = 0;

		/**
		 * Set the TexCoord padding. Normally, the processor will generate an impostor 
		 * with texcoords from 0.0 to 1.0, giving perfect UV coverage in the final texture. 
		 * If your engine uses permanent tiling rendering mode, you may need to add a 
		 * small padding to avoid sub-pixel wrapping artifact at the impostor edges, which 
		 * this setting supplies. Ex: Setting this to 0.01 will generate texcoords in 
		 * the range 0.01 to 0.99. 
		 * @param value is the value TexCoordPadding will be set to 
		 */
		virtual	void SetTexCoordPadding( real value ) = 0;

		/**
		 * Set the TexCoord padding. Normally, the processor will generate an impostor 
		 * with texcoords from 0.0 to 1.0, giving perfect UV coverage in the final texture. 
		 * If your engine uses permanent tiling rendering mode, you may need to add a 
		 * small padding to avoid sub-pixel wrapping artifact at the impostor edges, which 
		 * this setting supplies. Ex: Setting this to 0.01 will generate texcoords in 
		 * the range 0.01 to 0.99. 
		 * @return the current value of TexCoordPadding 
		 */
		virtual	real GetTexCoordPadding(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IImpostorSettings
	 */
	typedef CountedPointer<IImpostorSettings> spImpostorSettings;

	/// Conditional build define for the interface IImpostorSettings
	#define InterfaceDefined_SimplygonSDK_IImpostorSettings 1

	class IScene;
	class IGeometryData;
	class IMappingImageSettings;
	class IMappingImage;

	/**
	 * ISurfaceMapper creates a mapping image between two user-defined geometries. 
	 * The mapper basically looks for the source geometry in the inverse normal direction 
	 * from the outwardly offset destination geometry, and maps pixels accordingly. 
	 * This means that you can modify the normals of the destination geometry to adjust 
	 * the search direction to your liking. The geometries can either be set as IGeometryDatas 
	 * or as the root nodes of scenes. If both are set, the scene is used. 
	 */
	class ISurfaceMapper : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ISurfaceMapper is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ISurfaceMapper",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ISurfaceMapper pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ISurfaceMapper pointer
		 * @return a pointer to the ISurfaceMapper object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ISurfaceMapper *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ISurfaceMapper") )
				return static_cast<ISurfaceMapper*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene used for the surface transfer source 
		 * @param value is the source scene object 
		 */
		virtual	void SetSourceScene( IScene *value ) = 0;

		/**
		 * Get the scene used for the surface transfer source 
		 * @return the current top scene node 
		 */
		virtual	CountedPointer<IScene> GetSourceScene(  ) = 0;

		/**
		 * Set the scene used for the surface transfer destination 
		 * @param value is the destination scene object 
		 */
		virtual	void SetDestinationScene( IScene *value ) = 0;

		/**
		 * Get the scene used for vertex baking. 
		 * @return the current top scene node 
		 */
		virtual	CountedPointer<IScene> GetDestinationScene(  ) = 0;

		/**
		 * Set the source geometry data 
		 * @param value is the source geometry 
		 */
		virtual	void SetSourceGeometry( IGeometryData *value ) = 0;

		/**
		 * Get the source geometry data the surface transfer source 
		 * @return the current top scene node 
		 */
		virtual	CountedPointer<IGeometryData> GetSourceGeometry(  ) = 0;

		/**
		 * Set the destination geometry data 
		 * @param value is the destination geometry 
		 */
		virtual	void SetDestinationGeometry( IGeometryData *value ) = 0;

		/**
		 * Get the destination geometry data 
		 * @return the current top scene node 
		 */
		virtual	CountedPointer<IGeometryData> GetDestinationGeometry(  ) = 0;

		/**
		 * Get the mapping image settings. Currently, the only applicable settings for 
		 * this mapper is size, gutter, and supersampling settings. 
		 * @return the mapping image settings object 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * Get the mapping image. 
		 * @return the mapping image 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * Set the texcoord level in the destination geom to use for the material transfer 
		 * @param value is the texcoord level 
		 */
		virtual	void SetDestinationTexCoordSet( rid value ) = 0;

		/**
		 * Get the texcoord level in the destination geom to use for the material transfer 
		 * @return the texcoord level 
		 */
		virtual	rid GetDestinationTexCoordSet(  ) = 0;

		/**
		 * Set the RecalculateSearchDirection flag. If set, the search direction from 
		 * the destination mesh to the source mesh will be determined by an internal normal 
		 * calculation with the specified hard edge angle. If false, the search direction 
		 * will be determined by the existing normals of the destination geometry. 
		 * @param value is the desired flag value 
		 */
		virtual	void SetRecalculateSearchDirection( bool value ) = 0;

		/**
		 * Get the RecalculateSearchDirection flag. If set, the search direction from 
		 * the destination mesh to the source mesh will be determined by an internal normal 
		 * calculation with the specified hard edge angle. If false, the search direction 
		 * will be determined by the existing normals of the destination geometry. 
		 * @return the RecalculateSearchDirection flag 
		 */
		virtual	bool GetRecalculateSearchDirection(  ) = 0;

		/**
		 * Set the hard edge angle. If RecalculateSearchDirection is on, the search direction 
		 *  from the destination mesh to the source mesh will be determined by an internal 
		 * normal calculation with the specified hard edge angle. If false, the search 
		 * direction will be determined by the existing normals of the destination geometry. 
		 * @param value is the hard angle value 
		 */
		virtual	void SetSearchDirectionHardEdgeAngleInRadians( real value ) = 0;

		/**
		 * Get the hard edge angle. If RecalculateSearchDirection is set, the search direction 
		 *  from the destination mesh to the source mesh will be determined by an internal 
		 * normal calculation with the specified hard edge angle. If false, the search 
		 * direction will be determined by the existing normals of the destination geometry. 
		 * @return the hard edge angle 
		 */
		virtual	real GetSearchDirectionHardEdgeAngleInRadians(  ) = 0;

		/**
		 * Set the search distance. If the source geometry is found within this distance 
		 * from the destination geometry, it will be mapped. If negative, it will be set 
		 * to a sane default internally, based on the mesh size. 
		 * @param value is the search distance value 
		 */
		virtual	void SetSearchDistance( real value ) = 0;

		/**
		 * Get the RecalculateSearchDirection flag. If set, the search direction from 
		 * the destination mesh to the source mesh will be determined by an internal normal 
		 * calculation with the specified hard edge angle. 
		 * @return the search distance 
		 */
		virtual	real GetSearchDistance(  ) = 0;

		/**
		 * Set the search offset. This is how far out from the destination geometry the 
		 * search for the source geometry will begin, propagating inwardly for the length 
		 * of SearchDistance. If negative, it will be set to a sane default internally, 
		 * based on the mesh size. 
		 * @param value is the search offset value 
		 */
		virtual	void SetSearchOffset( real value ) = 0;

		/**
		 * Get the search offset. This is how far out from the destination geometry the 
		 * search for the source geometry will begin, propagating inwardly for the length 
		 * of SearchDistance. If negative, it will be set to a sane default internally, 
		 * based on the mesh size. 
		 * @return the search offset 
		 */
		virtual	real GetSearchOffset(  ) = 0;

		/**
		 * Runs the surface mapping 
		 */
		virtual	void RunSurfaceMapping(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ISurfaceMapper
	 */
	typedef CountedPointer<ISurfaceMapper> spSurfaceMapper;

	/// Conditional build define for the interface ISurfaceMapper
	#define InterfaceDefined_SimplygonSDK_ISurfaceMapper 1

	class IScene;
	class IOcclusionMeshSettings;

	/**
	 * The occlusion mesh processor creates a reconstruction of the input mesh from 
	 * it's silhouette. This means concavities and internal geometry disappear. WARNING: 
	 * Experimental. Generates nice meshes, but currently very slow at high settings. 
	 * Recommended onscreens size ~100 
	 */
	class IOcclusionMeshProcessor : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IOcclusionMeshProcessor is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IOcclusionMeshProcessor",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IOcclusionMeshProcessor pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IOcclusionMeshProcessor pointer
		 * @return a pointer to the IOcclusionMeshProcessor object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IOcclusionMeshProcessor *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IOcclusionMeshProcessor") )
				return static_cast<IOcclusionMeshProcessor*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene used for processing. All mesh nodes in the whole scene tree will 
		 * be replaced by one proxy mesh. Please note that internal refrences might be 
		 *  modified by the processor. The caller should refresh any reference that is 
		 * cached outside the scene after running the processor. 
		 * @param value is the scene node to which SceneRoot will be set 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene used for processing. All mesh nodes in the whole scene tree will 
		 * be replaced by one proxy mesh. Please note that internal refrences might be 
		 *  modified by the processor. The caller should refresh any reference that is 
		 * cached outside the scene after running the processor. 
		 * @return the scene root 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * The settings for the OcclusionMeshProcessor. 
		 * @return the ImpostorProcessor settings object associated with the processor 
		 *  
		 */
		virtual	CountedPointer<IOcclusionMeshSettings> GetOcclusionMeshSettings(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IOcclusionMeshProcessor
	 */
	typedef CountedPointer<IOcclusionMeshProcessor> spOcclusionMeshProcessor;

	/// Conditional build define for the interface IOcclusionMeshProcessor
	#define InterfaceDefined_SimplygonSDK_IOcclusionMeshProcessor 1


	/**
	 * Settings for occlusion meshes 
	 */
	class IOcclusionMeshSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IOcclusionMeshSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IOcclusionMeshSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IOcclusionMeshSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IOcclusionMeshSettings pointer
		 * @return a pointer to the IOcclusionMeshSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IOcclusionMeshSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IOcclusionMeshSettings") )
				return static_cast<IOcclusionMeshSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the onscreen size (px) of the output utility mesh. This will determine 
		 * triangle count and quality. 
		 * @param value the desired onscreen size 
		 */
		virtual	void SetOnScreenSize( unsigned int value ) = 0;

		/**
		 * Get the the onscreen size (px) of the output utility mesh. This will determine 
		 * triangle count and quality. 
		 * @return the current onscreen size 
		 */
		virtual	unsigned int GetOnScreenSize(  ) = 0;

		/**
		 * Set the OnScreenErrorTolerance. This determines how large of an error that 
		 * is tolerated, in pixels. Valid range is 1-50. Since this setting is based around 
		 * the absolute worst-case scenario, you can usually get good results at relatively 
		 * high tolerances. 
		 * @param value the desired OnScreenErrorTolerance 
		 */
		virtual	void SetOnScreenErrorTolerance( unsigned int value ) = 0;

		/**
		 * Get the OnScreenErrorTolerance. This determines how large of an error that 
		 * is tolerated, in pixels. Valid range is 1-50. Since this setting is based around 
		 * the absolute worst-case scenario, you can usually get good results at relatively 
		 * high tolerances. 
		 * @return the current OnScreenErrorTolerance 
		 */
		virtual	unsigned int GetOnScreenErrorTolerance(  ) = 0;

		/**
		 * Flips the winding of the triangles of the output, making frontfaces backfaces 
		 * and vice versa. Good for conservative z testing. 
		 * @param value the desired value of InvertOutputMesh 
		 */
		virtual	void SetInvertOutputMesh( bool value ) = 0;

		/**
		 * Flips the winding of the triangles of the output, making frontfaces backfaces 
		 * and vice versa. Good for conservative z testing. 
		 * @return the current value of InvertOutputMesh 
		 */
		virtual	bool GetInvertOutputMesh(  ) = 0;

		/**
		 * Sets whether to transfer the bone weights and bone ids to the new geometry 
		 * @param value the desired value of InvertOutputMesh 
		 */
		virtual	void SetTransferSkinning( bool value ) = 0;

		/**
		 * Gets whether to transfer the bone weights and bone ids to the new geometry 
		 * @return the current value of InvertOutputMesh 
		 */
		virtual	bool GetTransferSkinning(  ) = 0;

		/**
		 * Sets UseCameras. If set, cameras from the input scene will be used to define 
		 * what angles the occlusion mesh can be visible from, and optimize the output 
		 * mesh accordingly. Otherwise, the occlusion mesh will be assumed to be viewable 
		 * from all directions. 
		 * @param value the desired value of UseCameras 
		 */
		virtual	void SetUseCameras( bool value ) = 0;

		/**
		 * Gets UseCameras. If set, cameras from the input scene will be used to define 
		 * what angles the occlusion mesh can be visible from, and optimize the output 
		 * mesh accordingly. Otherwise, the occlusion mesh will be assumed to be viewable 
		 * from all directions. 
		 * @return the current value of UseCameras 
		 */
		virtual	bool GetUseCameras(  ) = 0;

		/**
		 * Get the camera selection set id. This determines the selection set used for 
		 * the cameras for when UseCameras is on. A value of -1 uses all cameras in the 
		 * scene. 
		 * @param value the desired OcclusionMode 
		 */
		virtual	void SetCameraSelectionSetId( int value ) = 0;

		/**
		 * Set the camera selection set id. This determines the selection set used for 
		 * the cameras for when UseCameras is on. A value of -1 uses all cameras in the 
		 * scene. 
		 * @return the current CameraSelectionSetId 
		 */
		virtual	int GetCameraSelectionSetId(  ) = 0;

		/**
		 * Set the camera selection set name. This determines the selection set used for 
		 * the cameras for when UseCameras is on. If set to NULL, all cameras in the scene 
		 * will be used. If both ID and Name are set, ID will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetCameraSelectionSetName( const char * value ) = 0;

		/**
		 * Get the camera selection set id. This determines the selection set used for 
		 * the cameras for when UseCameras is on. If set to NULL, all cameras in the scene 
		 * will be processed. If both ID and Name are set, ID will be used. 
		 * @return the current SelectionSet name 
		 */
		virtual	rstring GetCameraSelectionSetName(  ) = 0;

		/**
		 * Set the occlusion mode. This decides if the processor generates an occluder 
		 * (which is smaller than the original mesh), an ocludee (which is larger), or 
		 * an output that just tries to match the input. The options are: SG_OCCLUSIONMODE_OCCLUDER, 
		 * SG_OCCLUSIONMODE_OCLUDEE or SG_OCCLUSIONMODE_STANDARD 
		 * @param value the desired OcclusionMode 
		 */
		virtual	void SetOcclusionMode( unsigned int value ) = 0;

		/**
		 * Get the occlusion mode. This decides if the processor generates an occluder 
		 * (which is smaller than the original mesh), an ocludee (which is larger), or 
		 * an output that just tries to match the input. The options are: SG_OCCLUSIONMODE_OCCLUDER, 
		 * SG_OCCLUSIONMODE_OCLUDEE or SG_OCCLUSIONMODE_STANDARD 
		 * @return the current OcclusionMode 
		 */
		virtual	unsigned int GetOcclusionMode(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IOcclusionMeshSettings
	 */
	typedef CountedPointer<IOcclusionMeshSettings> spOcclusionMeshSettings;

	/// Conditional build define for the interface IOcclusionMeshSettings
	#define InterfaceDefined_SimplygonSDK_IOcclusionMeshSettings 1

	class IScene;
	class IFoliageSettings;
	class IMappingImageSettings;
	class IMappingImage;

	class IFoliageProcessor : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IFoliageProcessor is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IFoliageProcessor",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IFoliageProcessor pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IFoliageProcessor pointer
		 * @return a pointer to the IFoliageProcessor object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IFoliageProcessor *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IFoliageProcessor") )
				return static_cast<IFoliageProcessor*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene for the impostor processing. Please note that internal refrences 
		 * might be modified by the processor. The caller should refresh any reference 
		 * that is cached outside the scene after running the processor. 
		 * @param value is the scene 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene for the imposter processing. Please note that internal refrences 
		 * might be modified by the processor. The caller should refresh any reference 
		 * that is cached outside the scene after running the processor. 
		 * @return the current scene 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * The settings for the ImpostorProcessor. 
		 * @return the ImpostorProcessor settings object associated with the processor 
		 *  
		 */
		virtual	CountedPointer<IFoliageSettings> GetFoliageSettings(  ) = 0;

		/**
		 * The settings for the the MappingImage. 
		 * @return the MappingImage settings object associated with the processor 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetMappingImage GetMappingImage() \endlink instead. The MappingImage 
		 * for the Impostor geometry. 
		 * @return the MappingImage object associated with the processor 
		 */
		virtual	SGDEPRECATED CountedPointer<IMappingImage> GetFoliageMappingImage(  ) = 0;

		/**
		 * The MappingImage for the Impostor geometry. 
		 * @return the MappingImage object associated with the processor 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

		/**
		 * The 
		 * @return the 
		 */
		virtual	int GetFoliageMaterialID(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IFoliageProcessor
	 */
	typedef CountedPointer<IFoliageProcessor> spFoliageProcessor;

	/// Conditional build define for the interface IFoliageProcessor
	#define InterfaceDefined_SimplygonSDK_IFoliageProcessor 1


	/**
	 * Settings for the ImpostorProccessor 
	 */
	class IFoliageSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IFoliageSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IFoliageSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IFoliageSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IFoliageSettings pointer
		 * @return a pointer to the IFoliageSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IFoliageSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IFoliageSettings") )
				return static_cast<IFoliageSettings*>(ptr);
			return NULL;
			}

		/**
		 * Determines which type of Foliage impostor to generate. SG_FOLIAGETYPE_BILLBOARDS 
		 *  generates a static mesh with billboards. SG_FOLIAGETYPE_FLIPBOOK generates 
		 * a mapping image representing a number of views around the scene. 
		 * @return the current SG_FOLIAGETYPE_[...] value 
		 */
		virtual	unsigned int GetFoliageType(  ) = 0;

		/**
		 * Determines which type of Foliage impostor to generate. SG_FOLIAGETYPE_BILLBOARDS 
		 *  generates a static mesh with billboards. SG_FOLIAGETYPE_FLIPBOOK generates 
		 * a mapping image representing a number of views around the scene. 
		 * @param value is the SG_FOLIAGETYPE_[...] value 
		 */
		virtual	void SetFoliageType( unsigned int value ) = 0;

		/**
		 * Determines whether to prioritize generating vertical billboards which means 
		 * they will be optimized to be viewed from the side. Otherwise, if the foliage 
		 * in the input scene is mostly facing upwards/downwards then the generated billboards 
		 * will also be facing upwards/downwards which makes them not well suited to being 
		 * viewed from the side. 
		 * @return the current value 
		 */
		virtual	bool GetBillboardFavorVerticalPlanes(  ) = 0;

		/**
		 * Determines whether to prioritize generating vertical billboards which means 
		 * they will be optimized to be viewed from the side. Otherwise, if the foliage 
		 * in the input scene is mostly facing upwards/downwards then the generated billboards 
		 * will also be facing upwards/downwards which makes them not well suited to being 
		 * viewed from the side. 
		 * @param value the new value 
		 */
		virtual	void SetBillboardFavorVerticalPlanes( bool value ) = 0;

		/**
		 * Determines if billboards are limited to being planes or allowed to be more 
		 * complex polygons that follow the shape of the projected geometry. 
		 * @return the current value 
		 */
		virtual	bool GetBillboardAllowConvexPolygons(  ) = 0;

		/**
		 * Determines if billboards are limited to being planes or allowed to be more 
		 * complex polygons that follow the shape of the projected geometry. 
		 * @param value the new value 
		 */
		virtual	void SetBillboardAllowConvexPolygons( bool value ) = 0;

		/**
		 * If a billboard has multiple disjointed groups of foliage projected onto it 
		 * with spaces between, BillboardSubdividePlanes can be used to split the billboard 
		 * into multiple billboards that better encapsulates the projected foliage groups. 
		 * Subdividing the billboards will increase triangle count for the billboards, 
		 * but increase useful texture usage (less wasted space) and decrease rendering 
		 * overdraw. 
		 * @return the current value 
		 */
		virtual	bool GetBillboardSubdividePlanes(  ) = 0;

		/**
		 * If a billboard has multiple disjointed groups of foliage projected onto it 
		 * with spaces between, BillboardSubdividePlanes can be used to split the billboard 
		 * into multiple billboards that better encapsulates the projected foliage groups. 
		 * Subdividing the billboards will increase triangle count for the billboards, 
		 * but increase useful texture usage (less wasted space) and decrease rendering 
		 * overdraw. 
		 * @param value the new value 
		 */
		virtual	void SetBillboardSubdividePlanes( bool value ) = 0;

		/**
		 * Determines the maximum billboard count. When this criteria is met, it is not 
		 * guaranteed that all triangles in the scene have been accurately mapped to a 
		 * billboard yet. Triangles that have nor been mapped to a billboard within the 
		 * BillboardMaxDeviation distance will be mapped to an existing billboard that 
		 * best represents the triangle. 
		 * @return the current value 
		 */
		virtual	unsigned int GetBillboardMaxPlaneCount(  ) = 0;

		/**
		 * Determines the maximum billboard count. When this criteria is met, it is not 
		 * guaranteed that all triangles in the scene have been accurately mapped to a 
		 * billboard yet. Triangles that have nor been mapped to a billboard within the 
		 * BillboardMaxDeviation distance will be mapped to an existing billboard that 
		 * best represents the triangle. 
		 * @param value the new value 
		 */
		virtual	void SetBillboardMaxPlaneCount( unsigned int value ) = 0;

		/**
		 * Determines if the foliage is intended to be viewed from both sides without 
		 * back face culling. 
		 * @return the current value 
		 */
		virtual	bool GetBillboardTwoSided(  ) = 0;

		/**
		 * Determines if the foliage is intended to be viewed from both sides without 
		 * back face culling. 
		 * @param value the new value 
		 */
		virtual	void SetBillboardTwoSided( bool value ) = 0;

		/**
		 * Determines whether to prioritize accurately mapping triangles with higher visibility 
		 *  to billboards. 
		 * @return the current value 
		 */
		virtual	bool GetBillboardUseVisibilityWeights(  ) = 0;

		/**
		 * Determines whether to prioritize accurately mapping triangles with higher visibility 
		 *  to billboards. 
		 * @param value the new value 
		 */
		virtual	void SetBillboardUseVisibilityWeights( bool value ) = 0;

		/**
		 * Determines the number of views generated for the flip book impostor. 
		 * @return the current value 
		 */
		virtual	unsigned int GetFlipbookNumberOfViews(  ) = 0;

		/**
		 * Determines the number of views generated for the flip book impostor. 
		 * @param value the new value 
		 */
		virtual	void SetFlipbookNumberOfViews( unsigned int value ) = 0;

		/**
		 * Determines the vertical angle of the views to generate for the flip book impostor. 
		 *  0 radians: Looking horizontally at the scene. pi/2 radians: Looking at the 
		 * scene from above. -pi/2 radians: Looking at the scene from below. 
		 * @return the current value 
		 */
		virtual	float GetFlipbookVerticalAngleInRadians(  ) = 0;

		/**
		 * Determines the vertical angle of the views to generate for the flip book impostor. 
		 *  0 radians: Looking horizontally at the scene. pi/2 radians: Looking at the 
		 * scene from above. -pi/2 radians: Looking at the scene from below. 
		 * @param value the new value 
		 */
		virtual	void SetFlipbookVerticalAngleInRadians( float value ) = 0;

		/**
		 * Is used to separate the trunk from the foliage in a vegetation scene by comparing 
		 *  each group of connected triangles in the scene with the scene's largest group. 
		 * If a group's triangle count is below SeparateFoliageTriangleRatio * largestGrouptriangleCount 
		 *  it is treated as foliage and processed into billboards. The trunk is optimized 
		 * using triangle reduction. The range is 0.0->1.0. 0.0 means nothing will be 
		 * foliage, 1.0 means everything will be foliage. 
		 * @return the current value 
		 */
		virtual	float GetSeparateFoliageTriangleRatio(  ) = 0;

		/**
		 * Is used to separate the trunk from the foliage in a vegetation scene by comparing 
		 *  each group of connected triangles in the scene with the scene's largest group. 
		 * If a group's triangle count is below SeparateFoliageTriangleRatio * largestGrouptriangleCount 
		 *  it is treated as foliage and processed into billboards. The trunk is optimized 
		 * using triangle reduction. The range is 0.0->1.0. 0.0 means nothing will be 
		 * foliage, 1.0 means everything will be foliage. 
		 * @param value the new value 
		 */
		virtual	void SetSeparateFoliageTriangleRatio( float value ) = 0;

		/**
		 * Is used to separate the trunk from the foliage in a vegetation scene by finding 
		 *  each group of connected triangles in the scene and if the triangle count is 
		 * below SeparateFoliageTriangleThreshold it is treated as foliage and processed 
		 * into billboards. The trunk is optimized using reduction. The range is 0->large 
		 * value. 0 means nothing will be foliage, value larger than the maximum triangle 
		 * count means everything will be foliage. 
		 * @return the current value 
		 */
		virtual	unsigned int GetSeparateFoliageTriangleThreshold(  ) = 0;

		/**
		 * Is used to separate the trunk from the foliage in a vegetation scene by finding 
		 *  each group of connected triangles in the scene and if the triangle count is 
		 * below SeparateFoliageTriangleThreshold it is treated as foliage and processed 
		 * into billboards. The trunk is optimized using reduction. The range is 0->large 
		 * value. 0 means nothing will be foliage, value larger than the maximum triangle 
		 * count means everything will be foliage. 
		 * @param value the new value 
		 */
		virtual	void SetSeparateFoliageTriangleThreshold( unsigned int value ) = 0;

		/**
		 * Is used to separate the trunk from the foliage in a vegetation scene by finding 
		 *  each group of connected triangles in the scene and if the surface area is 
		 * below (SeparateFoliageAreaThreshold * AreaOfTheLargestGroupInTheScene) it is 
		 * treated as foliage and processed into billboards. The trunk is optimized using 
		 * reduction. The range is 0->large value. 0 means nothing will be foliage, value 
		 * larger than the maximum surface area means everything will be foliage. 
		 * @return the current value 
		 */
		virtual	real GetSeparateFoliageAreaThreshold(  ) = 0;

		/**
		 * Is used to separate the trunk from the foliage in a vegetation scene by finding 
		 *  each group of connected triangles in the scene and if the surface area is 
		 * below (SeparateFoliageAreaThreshold * AreaOfTheLargestGroupInTheScene) it is 
		 * treated as foliage and processed into billboards. The trunk is optimized using 
		 * reduction. The range is 0->large value. 0 means nothing will be foliage, value 
		 * larger than the maximum surface area means everything will be foliage. 
		 * @param value the new value 
		 */
		virtual	void SetSeparateFoliageAreaThreshold( real value ) = 0;

		/**
		 * Is used to separate the trunk from the foliage in a vegetation scene by finding 
		 *  each group of connected triangles in the scene and if the diameter of the 
		 * group is below (SeparateFoliageSizeThreshold * SceneDiameter) it is treated 
		 * as foliage and processed into billboards. The trunk is optimized using reduction. 
		 * The range is 0->large value. 0 means nothing will be foliage, value larger 
		 * than the scene's diameter means everything will be foliage. 
		 * @return the current value 
		 */
		virtual	real GetSeparateFoliageSizeThreshold(  ) = 0;

		/**
		 * Is used to separate the trunk from the foliage in a vegetation scene by finding 
		 *  each group of connected triangles in the scene and if the diameter of the 
		 * group is below (SeparateFoliageSizeThreshold * SceneDiameter) it is treated 
		 * as foliage and processed into billboards. The trunk is optimized using reduction. 
		 * The range is 0->large value. 0 means nothing will be foliage, value larger 
		 * than the scene's diameter means everything will be foliage. 
		 * @param value the new value 
		 */
		virtual	void SetSeparateFoliageSizeThreshold( real value ) = 0;

		/**
		 * Determines how many billboards to create. A large BillboardDensity means that 
		 * more billboards will be created to represent spatially spread out triangles 
		 * better. Having a low BillboardDensity means that fewer billboards will be created 
		 * and the distances from original leaves to billboards will get larger. The range 
		 * is 0->1. 
		 * @return the current value 
		 */
		virtual	float GetBillboardDensity(  ) = 0;

		/**
		 * Determines how many billboards to create. A large BillboardDensity means that 
		 * more billboards will be created to represent spatially spread out triangles 
		 * better. Having a low BillboardDensity means that fewer billboards will be created 
		 * and the distances from original leaves to billboards will get larger. The range 
		 * is 0->1. 
		 * @param value the new value 
		 */
		virtual	void SetBillboardDensity( float value ) = 0;

		/**
		 * Determines the triangle reduction ratio for the trunk part that has been separated 
		 *  from the foliage part based on the "separate foliage" settings. 
		 * @return the current value 
		 */
		virtual	float GetBillboardTrunkReductionRatio(  ) = 0;

		/**
		 * Determines the triangle reduction ratio for the trunk part that has been separated 
		 *  from the foliage part based on the "separate foliage" settings. 
		 * @param value the new value 
		 */
		virtual	void SetBillboardTrunkReductionRatio( float value ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IFoliageSettings
	 */
	typedef CountedPointer<IFoliageSettings> spFoliageSettings;

	/// Conditional build define for the interface IFoliageSettings
	#define InterfaceDefined_SimplygonSDK_IFoliageSettings 1

	class IScene;
	class IShadowMeshSettings;

	/**
	 * The shadow mesh processor creates shadow meshes for cheap shadow mapping, either 
	 *  from a specific direction or viewable from anywhere WARNING: Experimental. 
	 * Inconsistent results, and currently very slow at high settings. Recommended 
	 * onscreensize ~100. 
	 */
	class IShadowMeshProcessor : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadowMeshProcessor is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadowMeshProcessor",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadowMeshProcessor pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadowMeshProcessor pointer
		 * @return a pointer to the IShadowMeshProcessor object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadowMeshProcessor *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadowMeshProcessor") )
				return static_cast<IShadowMeshProcessor*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene used for processing. All mesh nodes in the whole scene tree will 
		 * be replaced by one proxy mesh. Please note that internal refrences might be 
		 *  modified by the processor. The caller should refresh any reference that is 
		 * cached outside the scene after running the processor. 
		 * @param value is the scene node to which SceneRoot will be set 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene used for processing. All mesh nodes in the whole scene tree will 
		 * be replaced by one proxy mesh. Please note that internal refrences might be 
		 *  modified by the processor. The caller should refresh any reference that is 
		 * cached outside the scene after running the processor. 
		 * @return the scene root 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * The settings for the ShadowMeshProcessor. 
		 * @return the ShadowMesh settings object associated with the processor 
		 */
		virtual	CountedPointer<IShadowMeshSettings> GetShadowMeshSettings(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadowMeshProcessor
	 */
	typedef CountedPointer<IShadowMeshProcessor> spShadowMeshProcessor;

	/// Conditional build define for the interface IShadowMeshProcessor
	#define InterfaceDefined_SimplygonSDK_IShadowMeshProcessor 1


	/**
	 * Settings for shadow meshes 
	 */
	class IShadowMeshSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IShadowMeshSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IShadowMeshSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IShadowMeshSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IShadowMeshSettings pointer
		 * @return a pointer to the IShadowMeshSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IShadowMeshSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IShadowMeshSettings") )
				return static_cast<IShadowMeshSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the onscreen size (px) of the output utility mesh. This will determine 
		 * triangle count and quality. 
		 * @param value the desired silhouette fidelity 
		 */
		virtual	void SetOnScreenSize( unsigned int value ) = 0;

		/**
		 * Get the the onscreen size (px) of the output utility mesh. This will determine 
		 * triangle count and quality. 
		 * @return the current silhouette fidelity 
		 */
		virtual	unsigned int GetOnScreenSize(  ) = 0;

		/**
		 * Set the fidelity. This determines how large of an error that is tolerated, 
		 * in pixels. Valid range is 1-50. Since this setting is based around the absolute 
		 * worst-case scenario, you can usually get good results at relatively high tolerances. 
		 * @param value the desired silhouette fidelity 
		 */
		virtual	void SetOnScreenErrorTolerance( unsigned int value ) = 0;

		/**
		 * Get the fidelity. This determines how large of an error that is tolerated, 
		 * in pixels. Valid range is 1-50. Since this setting is based around the absolute 
		 * worst-case scenario, you can usually get good results at relatively high tolerances. 
		 * @return the current silhouette fidelity 
		 */
		virtual	unsigned int GetOnScreenErrorTolerance(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IShadowMeshSettings
	 */
	typedef CountedPointer<IShadowMeshSettings> spShadowMeshSettings;

	/// Conditional build define for the interface IShadowMeshSettings
	#define InterfaceDefined_SimplygonSDK_IShadowMeshSettings 1

	class IImageData;
	class IBoolArray;

	/**
	 * compares images 
	 */
	class IImageComparer : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IImageComparer is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IImageComparer",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IImageComparer pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IImageComparer pointer
		 * @return a pointer to the IImageComparer object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IImageComparer *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IImageComparer") )
				return static_cast<IImageComparer*>(ptr);
			return NULL;
			}

		/**
		 * Sets the two images that will be compared. Both images needs to be of the same 
		 * type and the same size. 
		 * @param beforeImg the image data obejct for the before image 
		 * @param afterImg the image data obejct for the after image 
		 */
		virtual	void SetImages( IImageData *beforeImg , IImageData *afterImg ) = 0;

		/**
		 * Sets the two image-masks for the comparison. Both image-masks needs to be of 
		 * the same size as the compared images (same item-count as the number of pixels 
		 * in the images). True means a pixel in the compared image contains a part of 
		 * the rendered object, false means the pixel is only "background" and does not 
		 * need to be compared. When a pixel in the mask is set to "true" in one image 
		 * and "false" in the other image, this means that there is a silhouette-error. 
		 * How much this error should be measured can be set with the "SilhouetteDifferenceError" 
		 * setting. If masks are not set, all pixels in the images will be compared as 
		 * is, without taking into account what part of the images are of the actual model 
		 * and which parts are just background. 
		 * @param beforeImgMask the bool array for the before image mask 
		 * @param afterImgMask the bool array for the after image mask 
		 */
		virtual	void SetImageMasks( IBoolArray *beforeImgMask , IBoolArray *afterImgMask ) = 0;

		/**
		 * Compares the images either set from "SetImages" or the two images set from 
		 * the "ImagePathBeforeImg" and "ImagePathAfterImg" strings. Will return -1 if 
		 * two images were not set (in the same manner) <BR> Returns -2 if images aren't 
		 * of the same size <BR> Returns -3 if images are not 2D images (ImageData supports 
		 * 3D images for example - don't try to use these) <BR> Returns -4 if ImgMasks 
		 * are set, but are not same size as the Before and AfterImg. <BR> On success, 
		 * will return 0 
		 */
		virtual	int CompareImages(  ) = 0;

		/**
		 * Sets the path to one of the images to be compared. Both images needs to be 
		 * of the same type and size for this to work. This can be used instead of setting 
		 * the images from the "SetImage" function, and will only be used if the "SetImage" 
		 * function was never used. 
		 * @param value the file path to the image 
		 */
		virtual	void SetImagePathBeforeImg( const char * value ) = 0;

		/**
		 * Sets the path to the other of the images to be compared. Both images needs 
		 * to be of the same type and size for this to work. This can be used instead 
		 * of setting the images from the "SetImage" function, and will only be used if 
		 * the "SetImage" function was never used. 
		 * @param value the file path to the image 
		 */
		virtual	void SetImagePathAfterImg( const char * value ) = 0;

		/**
		 * Sets the export-path for the resulting comparison. Can be used to get a visual 
		 * feedback of the results of the comparison. 
		 * @param value the file path to the image 
		 */
		virtual	void SetImageDiffExportPath( const char * value ) = 0;

		/**
		 * Sets the export-path for the most heavily impacted areas of the comparison. 
		 * Can be used to get a visual feedback of the results of the comparison, to see 
		 * the highlighted "problem areas". 
		 * @param value the file path to the image 
		 */
		virtual	void SetDiffHighlightsExportPath( const char * value ) = 0;

		/**
		 * Returns the image for the resulting comparison. Can be used to get a visual 
		 * feedback of the results of the comparison. 
		 */
		virtual	CountedPointer<IImageData> GetImageDiff(  ) = 0;

		/**
		 * Returns the image for the most heavily impacted areas of the comparison. Can 
		 * be used to get a visual feedback of the results of the comparison, to see the 
		 * highlighted "problem areas". 
		 */
		virtual	CountedPointer<IImageData> GetDiffHighlights(  ) = 0;

		/**
		 * Returns the number of pixels that either the before- or the after-masks covers. 
		 *  Can be good to know when looking at the TotalPixelError and HighlightAreaCoverage 
		 *  numbers, to get a feel for the big the error of the image is relative to how 
		 * big the measured area actually was. 
		 */
		virtual	unsigned int GetTotalMaskCoverage(  ) = 0;

		/**
		 * Returns the size of the largest problem-areas. Preferably this value should 
		 * be not much larger than the 1-2 range, or the model should probably be further 
		 * away from the camera to get a good looking LOD-switch. 
		 */
		virtual	float GetMaxPixelError(  ) = 0;

		/**
		 * Return the sum of all pixel-errors of the whole image. Can be used to get a 
		 * feel for the over-all similarity between the two images. Especially when scaled 
		 * with the TotalMaskCoverage value. That way you can get the avarage pixel-error 
		 * of the model. 
		 */
		virtual	float GetTotalPixelError(  ) = 0;

		/**
		 * Returns the sum of all pixels that are in a "high-problem" area. Used together 
		 * with the TotalMaskCoverage value, this can let you know how large the problem-areas 
		 * are compared to the size of the model. 
		 */
		virtual	unsigned int GetHighlightAreaCoverage(  ) = 0;

		/**
		 * Decides how big of an impact the silhouette-difference between the two masks 
		 * will be. 0.0 means the silhouette difference will not be taken into account 
		 * at all, and 1.0 means it will be counted as maximum possible error. Default 
		 * is set to 0.5. This is because a lot of times the background behind the model 
		 * blends in somewhat with the model, and most times the difference in silhouette 
		 * will not be fully seen. 
		 * @param value the value of silhouette-difference (0.0 - 1.0) 
		 */
		virtual	void SetSilhouetteDifferenceError( float value ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IImageComparer
	 */
	typedef CountedPointer<IImageComparer> spImageComparer;

	/// Conditional build define for the interface IImageComparer
	#define InterfaceDefined_SimplygonSDK_IImageComparer 1

	class IGeometryData;

	/**
	 * Estimates a coarse recommended UV size (width and height) for a geometry based 
	 * on only its surface area and either the distance it is allowed to deviate or 
	 * its intended on screen size. The UVs are assumed to be unique i.e. not tiled/repeated. 
	 * RecommendedWidth and RecommendedHeight will only differ from each other if ForcePower2Texture 
	 * is enabled. 
	 */
	class IRecommendedTextureSizeCalculator : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IRecommendedTextureSizeCalculator is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IRecommendedTextureSizeCalculator",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IRecommendedTextureSizeCalculator pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IRecommendedTextureSizeCalculator pointer
		 * @return a pointer to the IRecommendedTextureSizeCalculator object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IRecommendedTextureSizeCalculator *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IRecommendedTextureSizeCalculator") )
				return static_cast<IRecommendedTextureSizeCalculator*>(ptr);
			return NULL;
			}

		/**
		 * If automatic_texture_size is enabled, then force the texture sizes to be a 
		 * power of 2 
		 * @param value the desired flag 
		 */
		virtual	void SetForcePower2Texture( bool value ) = 0;

		/**
		 * If automatic_texture_size is enabled, then force the texture sizes to be a 
		 * power of 2 
		 * @return the current flag 
		 */
		virtual	bool GetForcePower2Texture(  ) = 0;

		/**
		 * Set a texture dimension length multiplier for the automatic texture size. 
		 * @param value the desired multiplier 
		 */
		virtual	void SetRecommendedTextureSizeMultiplier( real value ) = 0;

		/**
		 * Get the current automatic texture size multiplier. 
		 * @return the current multiplier 
		 */
		virtual	real GetRecommendedTextureSizeMultiplier(  ) = 0;

		/**
		 * The recommended width calculated in CalculateRecommendedTextureSize() 
		 * @return the recommended width 
		 */
		virtual	unsigned int GetRecommendedWidth(  ) = 0;

		/**
		 * The recommended height calculated in CalculateRecommendedTextureSize() 
		 * @return the recommended height 
		 */
		virtual	unsigned int GetRecommendedHeight(  ) = 0;

		/**
		 * Calculates the recommended UV size based on the surface area of the geometry 
		 * and the allowed deviation. The recommended width and height can be fetched 
		 * with the GetRecommendedWidth() and GetRecommendedHeight() 
		 * @param geom the geometry for which to calculate the recommended UV size 
		 * @param deviation the deviation value 
		 */
		virtual	void CalculateBasedOnDeviation( IGeometryData *geom , real deviation ) = 0;

		/**
		 * Calculates the recommended UV size based on the surface area of the geometry 
		 * and the intended on screen size (in pixels) The recommended width and height 
		 * can be fetched with the GetRecommendedWidth() and GetRecommendedHeight() 
		 * @param geom the geometry for which to calculate the recommended UV size 
		 * @param onScreenSize the on screen size value 
		 */
		virtual	void CalculateBasedOnOnScreenSize( IGeometryData *geom , unsigned int onScreenSize ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IRecommendedTextureSizeCalculator
	 */
	typedef CountedPointer<IRecommendedTextureSizeCalculator> spRecommendedTextureSizeCalculator;

	/// Conditional build define for the interface IRecommendedTextureSizeCalculator
	#define InterfaceDefined_SimplygonSDK_IRecommendedTextureSizeCalculator 1

	class IGeometryData;
	class IScene;
	class IRealArray;

	/**
	 * The PartRemover tool can identify unconnected sub-meshes, and remove meshes 
	 * that fall below a set size threshold. This size threshold can be set globally 
	 * for the entire geometry, or per-material to be able to remove things like decals 
	 * effectively. 
	 */
	class IPartRemover : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IPartRemover is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IPartRemover",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IPartRemover pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IPartRemover pointer
		 * @return a pointer to the IPartRemover object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IPartRemover *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IPartRemover") )
				return static_cast<IPartRemover*>(ptr);
			return NULL;
			}

		/**
		 * Runs the part removal processing 
		 */
		virtual	void RunPartRemoval(  ) = 0;

		/**
		 * Set the geometry data object to run the part removal on. If a scene is set, 
		 * this parameter is ignored by the reducer. 
		 * @param value is the geometry data to be used for processing 
		 */
		virtual	void SetGeometry( IGeometryData *value ) = 0;

		/**
		 * Get the geometry data object to run the part removal on. If a scene is set, 
		 * this parameter is ignored by the reducer. 
		 * @return the geometry data to be used for processing 
		 */
		virtual	CountedPointer<IGeometryData> GetGeometry(  ) = 0;

		/**
		 * Set the scene object to run the part removal on. 
		 * @param value is the scene data to be used for processing 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene object to run the part removal on. 
		 * @return value is the scene data to be used for processing 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Set the size threshold to be used when removing parts. If SizeThreshold is 
		 * equal to, or greater than, the boundingbox radius of a part, then that part 
		 * is removed. If UsePerMaterialSizeThresholds is true, this value will be ignored 
		 * and the per-material values will be used instead. 
		 * @param value is the new value of SizeThreshold 
		 */
		virtual	void SetSizeThreshold( real value ) = 0;

		/**
		 * Get the size threshold to be used when removing parts. If SizeThreshold is 
		 * equal to, or greater than, the boundingbox radius of a part, then that part 
		 * is removed. If UsePerMaterialSizeThresholds is true, this value will be ignored 
		 * and the per-material values will be used instead. 
		 * @return the current value of SizeThreshold 
		 */
		virtual	real GetSizeThreshold(  ) = 0;

		/**
		 * Set the UsePerMaterialSizeThresholds flag. If true, the global SizeThreshold 
		 * value will be ignored and the per-material values in the PerMaterialSizeThresholds 
		 * will be used instead. 
		 * @param value is the new value of UsePerMaterialSizeThresholds 
		 */
		virtual	void SetUsePerMaterialSizeThresholds( bool value ) = 0;

		/**
		 * Get the UsePerMaterialSizeThresholds flag. If true, the global SizeThreshold 
		 * value will be ignored and the per-material values in the PerMaterialSizeThresholds 
		 * will be used instead. 
		 * @return the current value of UsePerMaterialSizeThresholds 
		 */
		virtual	bool GetUsePerMaterialSizeThresholds(  ) = 0;

		/**
		 * Get the per-material threshold array. These thresholds are mapped per material 
		 * ID in the geometrydata object if UsePerMaterialSizeThresholds is true. The 
		 * size and values in this array needs to be set up by the user, and material 
		 * IDs that fall outside the set array will be ignored. 
		 * @return the geometry data to be used for processing 
		 */
		virtual	CountedPointer<IRealArray> GetPerMaterialSizeThresholds(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IPartRemover
	 */
	typedef CountedPointer<IPartRemover> spPartRemover;

	/// Conditional build define for the interface IPartRemover
	#define InterfaceDefined_SimplygonSDK_IPartRemover 1

	class IScene;
	class IGeometryData;

	/**
	 * welds geometry vertices 
	 */
	class IWelder : public IProcessingObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IWelder is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IWelder",type)==0 )
				return true;
			return IProcessingObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IWelder pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IWelder pointer
		 * @return a pointer to the IWelder object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IWelder *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IWelder") )
				return static_cast<IWelder*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene 
		 * @param value the scene object 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene 
		 * @return the input scene 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Set the ID of the SelectionSet containing the meshes to weld If the ID is -1, 
		 * all meshes are selected 
		 * @param value the ID of the selection set to use 
		 */
		virtual	void SetSelectionSetID( rid value ) = 0;

		/**
		 * Set the ID of the SelectionSet containing the meshes to weld If the ID is -1, 
		 * all meshes are selected 
		 * @return the ID of the SelectionSet 
		 */
		virtual	rid GetSelectionSetID(  ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If ProcessSelectionSetName 
		 * is not found, all geometries in the scene will be processed. If both ID and 
		 * Name are set, ID will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetProcessSelectionSetName( const char * value ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to NULL, all geometries 
		 * in the scene will be processed. If both ID and Name are set, ID will be used. 
		 * @return the current SelectionSet name 
		 */
		virtual	rstring GetProcessSelectionSetName(  ) = 0;

		/**
		 * Set/Get the distance, below which, the vertices will be welded 
		 * @param value the weld distance 
		 */
		virtual	void SetWeldDist( real value ) = 0;

		/**
		 * Set/Get the distance, below which, the vertices will be welded 
		 */
		virtual	real GetWeldDist(  ) = 0;

		/**
		 * Welds the geometry 
		 * @param geom the geometry data object 
		 */
		virtual	void WeldGeometry( IGeometryData *geom ) = 0;

		/**
		 * UpdateExtents. If true, the welder will call CalculateExtents on the geometry 
		 * before welding. 
		 * @param value the desired flag value 
		 */
		virtual	void SetUpdateExtents( bool value ) = 0;

		/**
		 * UpdateExtents. If true, the welder will call CalculateExtents on the geometry 
		 * before welding. 
		 */
		virtual	bool GetUpdateExtents(  ) = 0;

		/**
		 * Sets the number of progressive passes. 
		 * @param value the number of passes to perform 
		 */
		virtual	void SetProgressivePasses( unsigned int value ) = 0;

		/**
		 * Returns the current number of progressive passes. 
		 */
		virtual	unsigned int GetProgressivePasses(  ) = 0;

		/**
		 * If IgnoreVertexLocks is true, then also vertices that are locked may be welded. 
		 * @param value the desired flag value 
		 */
		virtual	void SetIgnoreVertexLocks( bool value ) = 0;

		/**
		 * If IgnoreVertexLocks is true, then also vertices that are locked may be welded. 
		 *  
		 */
		virtual	bool GetIgnoreVertexLocks(  ) = 0;

		/**
		 * CompactMesh. If true, the welder will call Compact on the geometry after welding. 
		 * @param value the desired flag value 
		 */
		virtual	void SetCompactMesh( bool value ) = 0;

		/**
		 * CompactMesh. If true, the welder will call Compact on the geometry after welding. 
		 *  
		 */
		virtual	bool GetCompactMesh(  ) = 0;

		/**
		 * Set the OnlyBorder flag, if set, only vertices that are on the border are considered 
		 *  for the welding 
		 * @param value the new OnlyBorders value 
		 */
		virtual	void SetOnlyBorders( bool value ) = 0;

		/**
		 * Get the OnlyBorder flag, if set, only vertices that are on the border are considered 
		 *  for the welding 
		 * @return the current OnlyBorders value 
		 */
		virtual	bool GetOnlyBorders(  ) = 0;

		/**
		 * Set the OnlyWithinMaterial flag. If set, only vertices that are within the 
		 * same material are welded. Vertices on material borders are locked. 
		 * @param value the desired flag value 
		 */
		virtual	void SetOnlyWithinMaterial( bool value ) = 0;

		/**
		 * Get the OnlyWithinMaterial flag. If set, only vertices that are within the 
		 * same material are welded. Vertices on material borders are locked. 
		 * @return the current OnlyWithinMaterial value 
		 */
		virtual	bool GetOnlyWithinMaterial(  ) = 0;

		/**
		 * Set the OnlyWithinSceneNode flag. If set, only vertices that are within the 
		 * same scene node are welded. 
		 * @param value the desired flag value 
		 */
		virtual	void SetOnlyWithinSceneNode( bool value ) = 0;

		/**
		 * Get the OnlyWithinSceneNode flag. If set, only vertices that are within the 
		 * same scene node are welded. 
		 * @return the current OnlyWithinSceneNode 
		 */
		virtual	bool GetOnlyWithinSceneNode(  ) = 0;

		/**
		 * Set the OnlyBetweenSceneNodes flag. If set, only vertices that belong to different 
		 *  scene nodes are considered for the welding. 
		 * @param value the desired flag value 
		 */
		virtual	void SetOnlyBetweenSceneNodes( bool value ) = 0;

		/**
		 * Get the OnlyBetweenSceneNodes flag. If set, only vertices that belong to different 
		 *  scene nodes are considered for the welding. 
		 * @return the current OnlyBetweenSceneNodes value 
		 */
		virtual	bool GetOnlyBetweenSceneNodes(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetOnlyWithinSceneNode GetOnlyWithinSceneNode() \endlink instead. 
		 * Get the OnlyObjectBoundaryFlag flag. 
		 */
		virtual	SGDEPRECATED bool GetOnlyObjectBoundaryFlag(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link SetOnlyWithinSceneNode SetOnlyWithinSceneNode() \endlink instead. 
		 * Set the OnlyObjectBoundaryFlag flag. 
		 * @param onlyObjectBoundaryFlag the desired flag value 
		 */
		virtual	SGDEPRECATED void SetOnlyObjectBoundaryFlag( bool onlyObjectBoundaryFlag ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetOnlyWithinMaterial GetOnlyWithinMaterial() \endlink instead. Get 
		 * the OnlyWithinMaterialFlag flag. 
		 */
		virtual	SGDEPRECATED bool GetOnlyWithinMaterialFlag(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link SetOnlyWithinMaterial SetOnlyWithinMaterial() \endlink instead. Set 
		 * the OnlyWithinMaterialFlag flag. 
		 * @param onlyWithinMaterialFlag the desired flag value 
		 */
		virtual	SGDEPRECATED void SetOnlyWithinMaterialFlag( bool onlyWithinMaterialFlag ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetOnlyBorders GetOnlyBorders() \endlink instead. Get the OnlyBordersFlag 
		 * flag. 
		 */
		virtual	SGDEPRECATED bool GetOnlyBordersFlag(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link SetOnlyBorders SetOnlyBorders() \endlink instead. Set the OnlyBordersFlag 
		 * flag. 
		 * @param onlyBordersFlag the desired flag value 
		 */
		virtual	SGDEPRECATED void SetOnlyBordersFlag( bool onlyBordersFlag ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link GetProcessSelectionSetName GetProcessSelectionSetName() \endlink 
		 * instead. Get the name of the selection set used 
		 */
		virtual	SGDEPRECATED rstring GetSelectionSetName(  ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link SetProcessSelectionSetName SetProcessSelectionSetName() \endlink 
		 * instead. Set the name of the selection to use 
		 * @param selectionSetName the name of the selection set 
		 */
		virtual	SGDEPRECATED void SetSelectionSetName( const char * selectionSetName ) = 0;

		/**
		 * @deprecated Method is marked as deprecated and will be removed in future version. 
		 * Use \link RunProcessing RunProcessing() \endlink instead. Weld the geometry. 
		 * Alias for RunProcessing. 
		 */
		virtual	SGDEPRECATED void Weld(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IWelder
	 */
	typedef CountedPointer<IWelder> spWelder;

	/// Conditional build define for the interface IWelder
	#define InterfaceDefined_SimplygonSDK_IWelder 1

	class IScene;
	class IGeometryData;

	/**
	 * Removes t-junctions by subdividing triangles and welding vertices 
	 */
	class ITJunctionEliminator : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if ITJunctionEliminator is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("ITJunctionEliminator",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a ITJunctionEliminator pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a ITJunctionEliminator pointer
		 * @return a pointer to the ITJunctionEliminator object, if the cast can be made, and a NULL pointer otherwise
		 */
		static ITJunctionEliminator *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("ITJunctionEliminator") )
				return static_cast<ITJunctionEliminator*>(ptr);
			return NULL;
			}

		/**
		 * Set the scene 
		 * @param value the scene object 
		 */
		virtual	void SetScene( IScene *value ) = 0;

		/**
		 * Get the scene 
		 * @return the input scene 
		 */
		virtual	CountedPointer<IScene> GetScene(  ) = 0;

		/**
		 * Removes the t-junctions in the geometry data 
		 * @param geom the geometry data object 
		 */
		virtual	void RemoveTJunctions( IGeometryData *geom ) = 0;

		/**
		 * Removes the t-junctions in the set scene, processing each sub-geometry individually. 
		 *  
		 */
		virtual	void RemoveTJunctions(  ) = 0;

		/**
		 * Set/Get the distance, below which, the t-junctions will be welded 
		 * @param value the welding distance 
		 */
		virtual	void SetWeldDist( real value ) = 0;

		/**
		 * Set/Get the distance, below which, the t-junctions will be welded 
		 */
		virtual	real GetWeldDist(  ) = 0;

		/**
		 * Set/Get the OnlyObjectBoundary flag, if set, only vertices that are on the 
		 * boundary between two different objects are considered for the welding 
		 * @param value the desired value of the flag 
		 */
		virtual	void SetOnlyObjectBoundaryFlag( bool value ) = 0;

		/**
		 * Set/Get the OnlyObjectBoundary flag, if set, only vertices that are on the 
		 * boundary between two different objects are considered for the welding 
		 */
		virtual	bool GetOnlyObjectBoundaryFlag(  ) = 0;

		/**
		 * Set the ID of the SelectionSet containing the meshes to weld If the ID is -1, 
		 * all meshes are selected 
		 * @param value the ID of the selection set to use 
		 */
		virtual	void SetSelectionSetID( rid value ) = 0;

		/**
		 * Set the ID of the SelectionSet containing the meshes to weld If the ID is -1, 
		 * all meshes are selected 
		 * @return the ID of the SelectionSet 
		 */
		virtual	rid GetSelectionSetID(  ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If SetName is not found, all 
		 * geometries in the scene will be processed. If both ID and Name are set, ID 
		 * will be used. 
		 * @param value the desired SelectionSet name 
		 */
		virtual	void SetSelectionSetName( const char * value ) = 0;

		/**
		 * Selects which SelectionSet should be processed. If set to NULL, all geometries 
		 * in the scene will be processed. If both ID and Name are set, ID will be used. 
		 * @return the current SelectionSet name 
		 */
		virtual	rstring GetSelectionSetName(  ) = 0;

		/**
		 * Set/Get the OnlyWithinMaterial flag. If set, only vertices that are within 
		 * the same material are welded. Vertices on material borders are locked. 
		 * @param value the desired flag value 
		 */
		virtual	void SetOnlyWithinMaterialFlag( bool value ) = 0;

		/**
		 * Set/Get the OnlyWithinMaterial flag. If set, only vertices that are within 
		 * the same material are welded. Vertices on material borders are locked. 
		 */
		virtual	bool GetOnlyWithinMaterialFlag(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an ITJunctionEliminator
	 */
	typedef CountedPointer<ITJunctionEliminator> spTJunctionEliminator;

	/// Conditional build define for the interface ITJunctionEliminator
	#define InterfaceDefined_SimplygonSDK_ITJunctionEliminator 1

	class IScene;
	class IMaterialCaster;
	class IObjectCollection;
	class IPipelineSettings;

	/**
	 * IPipeline is the interface for a processing pipeline, which applies a number 
	 * of processing opearations on an input scene to generate an output scene. The 
	 * input and output scenes can be any file format supported by Simplygon. 
	 */
	class IPipeline : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IPipeline is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IPipeline",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IPipeline pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IPipeline pointer
		 * @return a pointer to the IPipeline object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IPipeline *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IPipeline") )
				return static_cast<IPipeline*>(ptr);
			return NULL;
			}

		/**
		 * Run the pipeline on the scene loaded from the given input scene file, and write 
		 *  the resulting scene to the given output scene file 
		 * @param input_scene The file path of the input scene file 
		 * @param output_scene The file path of the output scene file 
		 */
		virtual	void RunSceneFromFile( const char * input_scene , const char * output_scene ) = 0;

		/**
		 * Run the pipeline on the given scene, which will be modifed and contain the 
		 * result of the pipeline when the function returns. Please note that internal 
		 * refrences might be modified by the pipeline. The caller should refresh any 
		 * reference that is cached outside the scene after running the pipeline. 
		 * @param scene The scene to run the pipeline on. 
		 */
		virtual	void RunScene( IScene *scene ) = 0;

		/**
		 * Add a material caster. Material casting requires the mapping image generation 
		 * to be enabled (SetGenerateMappingImage in IMappingImageSettings returned by 
		 * GetMappingImageSettings) If called with a material index set to non-zero, the 
		 * mapping image for the corresponding output material will be used. See GetOutputMaterialCount/SetOutputMaterialCount 
		 * in IMappingImageSettings 
		 * @param MaterialCaster The material caster to add 
		 * @param MaterialIndex The material index the caster will be associated with, 
		 * 0 for first material 
		 */
		virtual	void AddMaterialCaster( IMaterialCaster *MaterialCaster , unsigned int MaterialIndex ) = 0;

		/**
		 * Add a material caster by type name. Type name should undecorated base name 
		 * of caster, for example "Color", "AmbientOcclusion", "Normal" etc. Material 
		 * casting requires the mapping image generation to be enabled (SetGenerateMappingImage 
		 * in IMappingImageSettings returned by GetMappingImageSettings). If called with 
		 * a material index set to non-zero, the mapping image for the corresponding output 
		 * material will be used. See GetOutputMaterialCount/SetOutputMaterialCount in 
		 * IMappingImageSettings 
		 * @param CasterType Caster type name, like "Color", "AmbientOcclusion", "Normal" 
		 * or similar 
		 * @param MaterialIndex The material index the caster will be associated with, 
		 * 0 for first material 
		 * @return The new material caster 
		 */
		virtual	CountedPointer<IMaterialCaster> AddMaterialCasterByType( const char * CasterType , unsigned int MaterialIndex ) = 0;

		/**
		 * Get the material casters for the pipeline 
		 * @return All material casters 
		 */
		virtual	CountedPointer<IObjectCollection> GetMaterialCasters(  ) = 0;

		/**
		 * Get the settings for the pipeline 
		 * @return Settings object for pipeline 
		 */
		virtual	CountedPointer<IPipelineSettings> GetPipelineSettings(  ) = 0;

		/**
		 * Set a named parameter boolean value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @param value New value for parameter 
		 * @return true if successful, false if error (invalid parameter path or mismatching 
		 * parameter type) 
		 */
		virtual	bool SetBoolParameter( const char * name , bool value ) = 0;

		/**
		 * Set a named parameter integer value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @param value New value for parameter 
		 * @return true if successful, false if error (invalid parameter path or mismatching 
		 * parameter type) 
		 */
		virtual	bool SetIntParameter( const char * name , int value ) = 0;

		/**
		 * Set a named parameter unsigned integer value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @param value New value for parameter 
		 * @return true if successful, false if error (invalid parameter path or mismatching 
		 * parameter type) 
		 */
		virtual	bool SetUIntParameter( const char * name , unsigned int value ) = 0;

		/**
		 * Set a named parameter floating point value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @param value New value for parameter 
		 * @return true if successful, false if error (invalid parameter path or mismatching 
		 * parameter type) 
		 */
		virtual	bool SetDoubleParameter( const char * name , double value ) = 0;

		/**
		 * Set a named parameter string value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @param value New value for parameter 
		 * @return true if successful, false if error (invalid parameter path or mismatching 
		 * parameter type) 
		 */
		virtual	bool SetStringParameter( const char * name , const char * value ) = 0;

		/**
		 * Set a named parameter value including parameter path. The string will be parsed 
		 * with best effort according to the parameter data type. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @param value New value for parameter 
		 * @return true if successful, false if error (invalid parameter path or mismatching 
		 * parameter type) 
		 */
		virtual	bool SetParameterFromString( const char * name , const char * value ) = 0;

		/**
		 * Get a named parameter boolean value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @return Parameter value, or false if invalid parameter path or mismatching 
		 * type 
		 */
		virtual	bool GetBoolParameter( const char * name ) = 0;

		/**
		 * Get a named parameter integer value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @return Parameter value, or 0 if invalid parameter path or mismatching type 
		 *  
		 */
		virtual	int GetIntParameter( const char * name ) = 0;

		/**
		 * Get a named parameter unsigned integer value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @return Parameter value, or 0 if invalid parameter path or mismatching type 
		 *  
		 */
		virtual	unsigned int GetUIntParameter( const char * name ) = 0;

		/**
		 * Get a named parameter floating point value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @return Parameter value, or 0 if invalid parameter path or mismatching type 
		 *  
		 */
		virtual	double GetDoubleParameter( const char * name ) = 0;

		/**
		 * Get a named parameter string value including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @return Parameter value, or null pointer if invalid parameter path or mismatching 
		 * type 
		 */
		virtual	rstring GetStringParameter( const char * name ) = 0;

		/**
		 * Get a named parameter data type including parameter path. 
		 * @param name Parameter path separated with forward slash, like "/RemeshingProcessor/Reduction/MergeGeometries" 
		 *  
		 * @return parameter data type (see SettingValueType enum), or SG_SETTINGVALUETYPE_INVALID 
		 * if invalid parameter path 
		 */
		virtual	unsigned int GetParameterType( const char * name ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IPipeline
	 */
	typedef CountedPointer<IPipeline> spPipeline;

	/// Conditional build define for the interface IPipeline
	#define InterfaceDefined_SimplygonSDK_IPipeline 1

	class IPipeline;

	class IPipelineSerializer : public IObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IPipelineSerializer is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IPipelineSerializer",type)==0 )
				return true;
			return IObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IPipelineSerializer pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IPipelineSerializer pointer
		 * @return a pointer to the IPipelineSerializer object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IPipelineSerializer *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IPipelineSerializer") )
				return static_cast<IPipelineSerializer*>(ptr);
			return NULL;
			}

		/**
		 * Load the pipeline definition to the given file and create a pipeline object 
		 * as defined in this file, including settings 
		 * @param path The file path of the input pipeline definition 
		 * @return A pipeline object with settings restored 
		 */
		virtual	CountedPointer<IPipeline> LoadPipelineFromFile( const char * path ) = 0;

		/**
		 * Save the pipeline definition and settings to the given file, which can be used 
		 * to restore the pipeline through the NewPipelineFromFile method 
		 * @param path The file path of the output pipeline definition 
		 * @param pipeline The pipeline object to save 
		 */
		virtual	void SavePipelineToFile( const char * path , IPipeline *pipeline ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IPipelineSerializer
	 */
	typedef CountedPointer<IPipelineSerializer> spPipelineSerializer;

	/// Conditional build define for the interface IPipelineSerializer
	#define InterfaceDefined_SimplygonSDK_IPipelineSerializer 1

	class IReductionSettings;
	class IRepairSettings;
	class INormalCalculationSettings;
	class IMappingImageSettings;
	class IVisibilitySettings;
	class IBoneSettings;
	class IMappingImage;

	/**
	 * IReductionPipeline provides a pipeline for a single reduction processor to be 
	 * applied on the input scene. 
	 */
	class IReductionPipeline : public IPipeline
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IReductionPipeline is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IReductionPipeline",type)==0 )
				return true;
			return IPipeline::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IReductionPipeline pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IReductionPipeline pointer
		 * @return a pointer to the IReductionPipeline object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IReductionPipeline *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IReductionPipeline") )
				return static_cast<IReductionPipeline*>(ptr);
			return NULL;
			}

		/**
		 * Get the reduction settings object containing the reduction settings for this 
		 * pipeline 
		 * @return the reduction settings 
		 */
		virtual	CountedPointer<IReductionSettings> GetReductionSettings(  ) = 0;

		/**
		 * Get the repair settings object containing the repair settings for this pipeline 
		 * @return the repair settings 
		 */
		virtual	CountedPointer<IRepairSettings> GetRepairSettings(  ) = 0;

		/**
		 * Get the normal calculation settings object containing the normal calculation 
		 * settings for this pipeline 
		 * @return the normal calculation settings 
		 */
		virtual	CountedPointer<INormalCalculationSettings> GetNormalCalculationSettings(  ) = 0;

		/**
		 * Get the mapping image settings object containing the mapping image settings 
		 * for this pipeline 
		 * @return the mapping image settings 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * Get the visibility settings object containing the visibility settings for this 
		 * pipeline 
		 * @return the visibility settings 
		 */
		virtual	CountedPointer<IVisibilitySettings> GetVisibilitySettings(  ) = 0;

		/**
		 * Get the bone settings object containing the bone settings for this pipeline 
		 * @return the bone settings 
		 */
		virtual	CountedPointer<IBoneSettings> GetBoneSettings(  ) = 0;

		/**
		 * Get the mapping image from the performed reduction. Should only be called after 
		 *  the pipeline has been run 
		 * @return the mapping image for the reduction 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IReductionPipeline
	 */
	typedef CountedPointer<IReductionPipeline> spReductionPipeline;

	/// Conditional build define for the interface IReductionPipeline
	#define InterfaceDefined_SimplygonSDK_IReductionPipeline 1

	class IRemeshingSettings;
	class IMappingImageSettings;
	class IVisibilitySettings;
	class IBoneSettings;
	class IMappingImage;

	/**
	 * IRemeshingPipeline provides a pipeline for a single remeshing processor to be 
	 * applied on the input scene. 
	 */
	class IRemeshingPipeline : public IPipeline
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IRemeshingPipeline is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IRemeshingPipeline",type)==0 )
				return true;
			return IPipeline::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IRemeshingPipeline pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IRemeshingPipeline pointer
		 * @return a pointer to the IRemeshingPipeline object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IRemeshingPipeline *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IRemeshingPipeline") )
				return static_cast<IRemeshingPipeline*>(ptr);
			return NULL;
			}

		/**
		 * Get the remeshing settings object containing the reduction settings for this 
		 * pipeline 
		 * @return the reduction settings 
		 */
		virtual	CountedPointer<IRemeshingSettings> GetRemeshingSettings(  ) = 0;

		/**
		 * Get the mapping image settings object containing the mapping image settings 
		 * for this pipeline 
		 * @return the mapping image settings 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * Get the visibility settings object containing the visibility settings for this 
		 * pipeline 
		 * @return the visibility settings 
		 */
		virtual	CountedPointer<IVisibilitySettings> GetVisibilitySettings(  ) = 0;

		/**
		 * Get the bone settings object containing the bone settings for this pipeline 
		 * @return the bone settings 
		 */
		virtual	CountedPointer<IBoneSettings> GetBoneSettings(  ) = 0;

		/**
		 * Get the mapping image from the performed reduction. Should only be called after 
		 *  the pipeline has been run 
		 * @return the mapping image for the reduction 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IRemeshingPipeline
	 */
	typedef CountedPointer<IRemeshingPipeline> spRemeshingPipeline;

	/// Conditional build define for the interface IRemeshingPipeline
	#define InterfaceDefined_SimplygonSDK_IRemeshingPipeline 1

	class IAggregationSettings;
	class IMappingImageSettings;
	class IVisibilitySettings;
	class IMappingImage;

	/**
	 * IAggregationPipeline provides a pipeline for a single aggregation processor 
	 * to be applied on the input scene. 
	 */
	class IAggregationPipeline : public IPipeline
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IAggregationPipeline is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IAggregationPipeline",type)==0 )
				return true;
			return IPipeline::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IAggregationPipeline pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IAggregationPipeline pointer
		 * @return a pointer to the IAggregationPipeline object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IAggregationPipeline *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IAggregationPipeline") )
				return static_cast<IAggregationPipeline*>(ptr);
			return NULL;
			}

		/**
		 * Get the remeshing settings object containing the reduction settings for this 
		 * pipeline 
		 * @return the reduction settings 
		 */
		virtual	CountedPointer<IAggregationSettings> GetAggregationSettings(  ) = 0;

		/**
		 * Get the mapping image settings object containing the mapping image settings 
		 * for this pipeline 
		 * @return the mapping image settings 
		 */
		virtual	CountedPointer<IMappingImageSettings> GetMappingImageSettings(  ) = 0;

		/**
		 * Get the visibility settings object containing the visibility settings for this 
		 * pipeline 
		 * @return the visibility settings 
		 */
		virtual	CountedPointer<IVisibilitySettings> GetVisibilitySettings(  ) = 0;

		/**
		 * Get the mapping image from the performed reduction. Should only be called after 
		 *  the pipeline has been run 
		 * @return the mapping image for the reduction 
		 */
		virtual	CountedPointer<IMappingImage> GetMappingImage(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IAggregationPipeline
	 */
	typedef CountedPointer<IAggregationPipeline> spAggregationPipeline;

	/// Conditional build define for the interface IAggregationPipeline
	#define InterfaceDefined_SimplygonSDK_IAggregationPipeline 1


	/**
	 * IPipelineSettings is the interface used to manage settings for a pipeline 
	 */
	class IPipelineSettings : public ISettingsObject
	{
	public:
		/**
		 * GetClass returns the name of the class of the object.
		 * @return the name of the actual class of the object, as a const char string
		 */
		virtual const char *GetClass();

		/** 
		 * The IsA function returns true if the object is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the object is, or is a descendant of
		 * @return true if the object is of the specified class, false if not
		 */
		virtual bool IsA( const char *type ) const;

		/**
		 * The IsClassA function returns true if IPipelineSettings is a or is a descendant of the class named as the type parameter
		 * @param type is the name of the class to check if the class is, or is a descendant of
		 * @return true if the class is of the specified class, false if not
		 */
		static bool IsClassA( const char *type )
			{
			if( !type )
				return false;
			if( strcmp("IPipelineSettings",type)==0 )
				return true;
			return ISettingsObject::IsClassA(type);
			}
		/** 
		 * SafeCast makes sure the pointer is of a class that can be cast into a IPipelineSettings pointer, and if this is possible, returns the cast pointer. 
		 * @param ptr is the pointer to be cast into a IPipelineSettings pointer
		 * @return a pointer to the IPipelineSettings object, if the cast can be made, and a NULL pointer otherwise
		 */
		static IPipelineSettings *SafeCast( IObject *ptr )
			{
			if( ptr && ptr->IsA("IPipelineSettings") )
				return static_cast<IPipelineSettings*>(ptr);
			return NULL;
			}

		/**
		 * Set the path where the pipeline will save any created textures from material 
		 * casters. A relative path will be treated as relative to the scene file output 
		 * path if the pipeline is executed with scene file paths (RunSceneFromFile), 
		 * or from the current working directory if the pipeline is executed with a scene 
		 * object (RunScene). The directory must exist, it will not be created. 
		 * @param value Path where created textures will be saved 
		 */
		virtual	void SetTextureOutputPath( const char * value ) = 0;

		/**
		 * Get the path where the pipeline will save any created textures from material 
		 * casters. See SetTextureOutputPath for documentation regarding relative paths. 
		 * @return Path where created textures will be saved 
		 */
		virtual	rstring GetTextureOutputPath(  ) = 0;

	};

	/**
	 * a CounterPointer smart pointer to an IPipelineSettings
	 */
	typedef CountedPointer<IPipelineSettings> spPipelineSettings;

	/// Conditional build define for the interface IPipelineSettings
	#define InterfaceDefined_SimplygonSDK_IPipelineSettings 1

	class ISimplygonSDK
	{
	public:
		/**
		 * Creates and returns a counted pointer to an object of type IStringArray
		 * Documentation from IStringArray:
		 * @deprecated Interface is deprecated and will be removed in a future release.Deprecated 
		 * interface, which will be removed in version 9.0. ReStringArray is the string 
		 * implementation of ReValueArray. See ReValueArray for a description. Please 
		 *  note that conversions to/from real values will not work on string arrays. 
		 *  
		 * @return the created object, of type spStringArray
		 */
		SGDEPRECATED virtual CountedPointer<IStringArray> CreateStringArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IBoolArray
		 * Documentation from IBoolArray:
		 * IBoolArray is the bool implementation of IValueArray. See IValueArray for a 
		 * description. 
		 * @return the created object, of type spBoolArray
		 */
		virtual CountedPointer<IBoolArray> CreateBoolArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ICharArray
		 * Documentation from ICharArray:
		 * ICharArray is the char implementation of IValueArray. See IValueArray for a 
		 * description. 
		 * @return the created object, of type spCharArray
		 */
		virtual CountedPointer<ICharArray> CreateCharArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IDoubleArray
		 * Documentation from IDoubleArray:
		 * IDoubleArray is the double implementation of IValueArray. See IValueArray for 
		 * a description. 
		 * @return the created object, of type spDoubleArray
		 */
		virtual CountedPointer<IDoubleArray> CreateDoubleArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IFloatArray
		 * Documentation from IFloatArray:
		 * IFloatArray is the float implementation of IValueArray. See IValueArray for 
		 * a description. 
		 * @return the created object, of type spFloatArray
		 */
		virtual CountedPointer<IFloatArray> CreateFloatArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IIntArray
		 * Documentation from IIntArray:
		 * IIntArray is the int implementation of IValueArray. See IValueArray for a description. 
		 *  
		 * @return the created object, of type spIntArray
		 */
		virtual CountedPointer<IIntArray> CreateIntArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ILongArray
		 * Documentation from ILongArray:
		 * ILongArray is the long implementation of IValueArray. See IValueArray for a 
		 * description. 
		 * @return the created object, of type spLongArray
		 */
		virtual CountedPointer<ILongArray> CreateLongArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IRealArray
		 * Documentation from IRealArray:
		 * IRealArray is the real implementation of IValueArray. See IValueArray for a 
		 * description. 
		 * @return the created object, of type spRealArray
		 */
		virtual CountedPointer<IRealArray> CreateRealArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IRidArray
		 * Documentation from IRidArray:
		 * IRidArray is the rid implementation of IValueArray. See IValueArray for a description. 
		 *  
		 * @return the created object, of type spRidArray
		 */
		virtual CountedPointer<IRidArray> CreateRidArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShortArray
		 * Documentation from IShortArray:
		 * IShortArray is the short implementation of IValueArray. See IValueArray for 
		 * a description. 
		 * @return the created object, of type spShortArray
		 */
		virtual CountedPointer<IShortArray> CreateShortArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IUnsignedCharArray
		 * Documentation from IUnsignedCharArray:
		 * IUnsignedCharArray is the uchar implementation of IValueArray. See IValueArray 
		 * for a description. 
		 * @return the created object, of type spUnsignedCharArray
		 */
		virtual CountedPointer<IUnsignedCharArray> CreateUnsignedCharArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IUnsignedIntArray
		 * Documentation from IUnsignedIntArray:
		 * IUnsignedIntArray is the uint implementation of IValueArray. See IValueArray 
		 * for a description. 
		 * @return the created object, of type spUnsignedIntArray
		 */
		virtual CountedPointer<IUnsignedIntArray> CreateUnsignedIntArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IUnsignedLongArray
		 * Documentation from IUnsignedLongArray:
		 * IUnsignedLongArray is the ulong implementation of IValueArray. See IValueArray 
		 * for a description. 
		 * @return the created object, of type spUnsignedLongArray
		 */
		virtual CountedPointer<IUnsignedLongArray> CreateUnsignedLongArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IUnsignedShortArray
		 * Documentation from IUnsignedShortArray:
		 * IUnsignedShortArray is the ushort implementation of IValueArray. See IValueArray 
		 * for a description. 
		 * @return the created object, of type spUnsignedShortArray
		 */
		virtual CountedPointer<IUnsignedShortArray> CreateUnsignedShortArray() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IDirectXRenderer
		 * Documentation from IDirectXRenderer:
		 * A Renderer using DirectX 11 that can be used to preview a scene object containing 
		 *  geometry data by rendering it from selected SceneCamera nodes within the given 
		 * scene and then storing the frames to disc. If using a Shading Node Network 
		 * (having an IShadingNode assigned to the IMaterial), then the material can be 
		 * previewed with the generated HLSL shader. 
		 * @return the created object, of type spDirectXRenderer
		 */
		virtual CountedPointer<IDirectXRenderer> CreateDirectXRenderer() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IGeometryData
		 * Documentation from IGeometryData:
		 * IGeometryData represents a geometric structure consisting of point data (Vertices) 
		 *  and topological data (Triangles). IGeometryData may represent a whole geometric 
		 *  object, but can also be used by streaming data filters, and in this way, only 
		 * represents a part of the object. <br><br> Different fields in the Vertices 
		 * and Triangles fields data objects will contain point data and topological data. 
		 * The standard naming convention used in the filters/renderers are as follows: 
		 * (Case sensitive naming, other fields may also be present) <br><br> 'Coords' 
		 * <br> Positional coordinates of a vertex, expressed with a 3-component real 
		 * field (XYZ). <br><br> 'TexCoords0' - 'TexCoords255' <br> Texture coordinates 
		 * of a vertex, expressed with a 2 components real field. By convention, the existing 
		 * fields must be sequential, and must start with 'TexCoord0'. <br><br> 'Normals' 
		 * <br> Normal vector for the vertex, expressed with a 3-component real field, 
		 * a normalized (XYZ) vector. <br><br> 'VertexIds' <br> The id of the primitive's 
		 *  current vertex. This field is present even if the vertex data is directly 
		 * specified in the primitive's data, to specify topology. The field is of type 
		 * rid. <br><br> 'MaterialIds' <br> The material of the primitive. The field is 
		 * of type rid. <br><br> The 'Coords' and 'VertexIds' fields always exist in the 
		 * object, but the other fields are optional. Also, there can exist user fields. 
		 *  
		 * @return the created object, of type spGeometryData
		 */
		virtual CountedPointer<IGeometryData> CreateGeometryData() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IPackedGeometryData
		 * Documentation from IPackedGeometryData:
		 * IPackedGeometryData keeps the same information as IGeometryData, but with all 
		 * corner data fields moved into per-vertex fields. No per-corner fields exist, 
		 * apart from the VertexIds field. Please note that the number of vertices in 
		 * an IPackedGeometryData is commonly higher than in an IGeometryData, as vertices 
		 * must be split to accommodate for different corner data. 
		 * @return the created object, of type spPackedGeometryData
		 */
		virtual CountedPointer<IPackedGeometryData> CreatePackedGeometryData() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IImageData
		 * Documentation from IImageData:
		 * IImageData holds unpacked image data. The data is stored in an IFieldData object 
		 *  as value fields. For simplicity all images are implemented as 3D images, but 
		 * has an extent of 1 in Z for 2D images, and an extent of 1 in both Y and Z for 
		 * 1D images. Cube maps are stored as a 3D image with Z-depth of 6 (one for each 
		 * side) 
		 * @return the created object, of type spImageData
		 */
		virtual CountedPointer<IImageData> CreateImageData() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ITable
		 * Documentation from ITable:
		 * ITable is the base class for tables objects, where items are added into an 
		 * id-indexed array of objects. 
		 * @return the created object, of type spTable
		 */
		virtual CountedPointer<ITable> CreateTable() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IMaterial
		 * Documentation from IMaterial:
		 * IMaterial keeps information on materials used while rendering. 
		 * @return the created object, of type spMaterial
		 */
		virtual CountedPointer<IMaterial> CreateMaterial() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IMaterialTable
		 * Documentation from IMaterialTable:
		 * IMaterialTable keeps information on materials used while rendering. 
		 * @return the created object, of type spMaterialTable
		 */
		virtual CountedPointer<IMaterialTable> CreateMaterialTable() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IMatrix4x4
		 * Documentation from IMatrix4x4:
		 * IMatrix4x4 is used to represent and manipulate 4x4 transformation matrices, 
		 * which can be either standard matrices, or homogeneous 4x4 matrices used to 
		 * transform 3D homogeneous coordinates [x y z w]. The transformations are defined 
		 * in row-major order. 
		 * @return the created object, of type spMatrix4x4
		 */
		virtual CountedPointer<IMatrix4x4> CreateMatrix4x4() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ITransform3
		 * Documentation from ITransform3:
		 * ITransform3 handles homogeneous 4x4 transforms, i.e. transformations which 
		 * can be represented by multiplying a 4x4 matrix with a homogeneous 3D coordinate. 
		 * <br> ITransform3 can either pre-multiply a matrix onto the transformation, 
		 * which will add a transform around the current transform, or it can post-multiply 
		 * the matrix, which will add a transform inside the current transform. Post-multiply 
		 * is the default mode. 
		 * @return the created object, of type spTransform3
		 */
		virtual CountedPointer<ITransform3> CreateTransform3() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IParameterizer
		 * Documentation from IParameterizer:
		 * Computes texture coordinates for arbitrary geometry. 
		 * @return the created object, of type spParameterizer
		 */
		virtual CountedPointer<IParameterizer> CreateParameterizer() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IObjectCollection
		 * Documentation from IObjectCollection:
		 * IObjectCollection and its specializations handles a collection of IObjects. 
		 * There are methods for adding, removing and iterating through the objects. 
		 * @return the created object, of type spObjectCollection
		 */
		virtual CountedPointer<IObjectCollection> CreateObjectCollection() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IGeometryDataCollection
		 * Documentation from IGeometryDataCollection:
		 * IGeometryDataCollection holds a number of IGeometryData objects. The objects 
		 * can be looked up by their names (if they have been named through IObject::SetName() 
		 *  ). 
		 * @return the created object, of type spGeometryDataCollection
		 */
		virtual CountedPointer<IGeometryDataCollection> CreateGeometryDataCollection() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IWavefrontImporter
		 * Documentation from IWavefrontImporter:
		 * base class for graphics import classes 
		 * @return the created object, of type spWavefrontImporter
		 */
		virtual CountedPointer<IWavefrontImporter> CreateWavefrontImporter() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IWavefrontExporter
		 * Documentation from IWavefrontExporter:
		 * Wavefront .obj geometry exporter 
		 * @return the created object, of type spWavefrontExporter
		 */
		virtual CountedPointer<IWavefrontExporter> CreateWavefrontExporter() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IBinaryImporter
		 * Documentation from IBinaryImporter:
		 * IBinaryImporter loads the geometry data from a binary file, that is specific 
		 * for Simplygon. Please note that the binary file is only intended for temporary 
		 * storage, and that no compatibility between versions of Simplygon is guaranteed. 
		 *  
		 * @return the created object, of type spBinaryImporter
		 */
		virtual CountedPointer<IBinaryImporter> CreateBinaryImporter() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IBinaryExporter
		 * Documentation from IBinaryExporter:
		 * IBinaryExporter stores the geometry data into a binary file, that is specific 
		 * for Simplygon. Please note that the binary file is only intended for temporary 
		 * storage, and that no compatibility between versions of Simplygon is guaranteed. 
		 *  
		 * @return the created object, of type spBinaryExporter
		 */
		virtual CountedPointer<IBinaryExporter> CreateBinaryExporter() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IGeometryValidator
		 * Documentation from IGeometryValidator:
		 * Checks the geometry for consistent indices, number of triangles etc. 
		 * @return the created object, of type spGeometryValidator
		 */
		virtual CountedPointer<IGeometryValidator> CreateGeometryValidator() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IMaterialCasterSettings
		 * Documentation from IMaterialCasterSettings:
		 * IMaterialCasterSettings is the base interface for material casting settings 
		 *  
		 * @return the created object, of type spMaterialCasterSettings
		 */
		virtual CountedPointer<IMaterialCasterSettings> CreateMaterialCasterSettings() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IColorCaster
		 * Documentation from IColorCaster:
		 * IColorCaster is the interface used to cast basic color values. Currently supported 
		 *  material color value types are Ambient, Diffuse and Specular + Shininess. 
		 *  
		 * @return the created object, of type spColorCaster
		 */
		virtual CountedPointer<IColorCaster> CreateColorCaster() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IColorCasterSettings
		 * Documentation from IColorCasterSettings:
		 * IColorCasterSettings is the interface for basic color caster settings 
		 * @return the created object, of type spColorCasterSettings
		 */
		virtual CountedPointer<IColorCasterSettings> CreateColorCasterSettings() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IOpacityCaster
		 * Documentation from IOpacityCaster:
		 * IOpacityCaster is the interface used to cast opacity values. To be used when 
		 * opacity is needed as a separate texture. 
		 * @return the created object, of type spOpacityCaster
		 */
		virtual CountedPointer<IOpacityCaster> CreateOpacityCaster() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IOpacityCasterSettings
		 * Documentation from IOpacityCasterSettings:
		 * IOpacityCasterSettings is the interface used to manage settings for an opacity 
		 * caster 
		 * @return the created object, of type spOpacityCasterSettings
		 */
		virtual CountedPointer<IOpacityCasterSettings> CreateOpacityCasterSettings() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type INormalCaster
		 * Documentation from INormalCaster:
		 * INormalCaster is the interface used to cast normals data onto the receiving 
		 * geometry. 
		 * @return the created object, of type spNormalCaster
		 */
		virtual CountedPointer<INormalCaster> CreateNormalCaster() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type INormalCasterSettings
		 * Documentation from INormalCasterSettings:
		 * INormalCasterSettings is the interface used to manage settings for a normal 
		 * caster 
		 * @return the created object, of type spNormalCasterSettings
		 */
		virtual CountedPointer<INormalCasterSettings> CreateNormalCasterSettings() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type INormalRepairer
		 * Documentation from INormalRepairer:
		 * Repairs normals on a processed geometry. 
		 * @return the created object, of type spNormalRepairer
		 */
		virtual CountedPointer<INormalRepairer> CreateNormalRepairer() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IDisplacementCaster
		 * Documentation from IDisplacementCaster:
		 * IDisplacementCaster is used to store the delta-values between the original 
		 * and processed geometries. The values are divided by a scaling value, and clamped 
		 * into the -1 -> 1 range before being stored into an image. 
		 * @return the created object, of type spDisplacementCaster
		 */
		virtual CountedPointer<IDisplacementCaster> CreateDisplacementCaster() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IDisplacementCasterSettings
		 * Documentation from IDisplacementCasterSettings:
		 * IDisplacementCasterSettings is used to store the settings for a displacement 
		 * caster 
		 * @return the created object, of type spDisplacementCasterSettings
		 */
		virtual CountedPointer<IDisplacementCasterSettings> CreateDisplacementCasterSettings() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ITangentCalculator
		 * Documentation from ITangentCalculator:
		 * IGeometryTangentCalculator calculates tangent vectors for geometry data objects. 
		 *  One corner field of texture coordinates must exist, as well as normals. The 
		 * Tangents are placed into two corner fields called "Tangents" and "Bitangents". 
		 * If any of these fields already exists, it is replaced by the new field. 
		 * @return the created object, of type spTangentCalculator
		 */
		virtual CountedPointer<ITangentCalculator> CreateTangentCalculator() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IReductionSettings
		 * Documentation from IReductionSettings:
		 * Settings for the IReductionProcessor class. Handles all the settings for Simplygon’s 
		 *  simplification algorithms. The user can tell the reduction processor which 
		 * features are important by using the “FeatureFlags” parameter, and also the 
		 * relative importance of these features using the Set{feature}Importance methods. 
		 * Also settings for things like symmetry are set here. 
		 * @return the created object, of type spReductionSettings
		 */
		virtual CountedPointer<IReductionSettings> CreateReductionSettings() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IReductionProcessor
		 * Documentation from IReductionProcessor:
		 * IReductionProcessor welds, removes t-junctions, removes double-triangles and 
		 * reduces the trianglecount of the Geometry. It also calculates new normals for 
		 * the reduced and optimized Geometry. 
		 * @return the created object, of type spReductionProcessor
		 */
		virtual CountedPointer<IReductionProcessor> CreateReductionProcessor() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IRemeshingProcessor
		 * Documentation from IRemeshingProcessor:
		 * IRemeshingProcessor creates a new geometry that is based on the input geometry, 
		 *  but that is topologically based on the size of the input geometry rendered 
		 * at a specific screen size, and allowed pixel error. The remesher will on demand 
		 * also add texture coordinates and a mapping texture to the output remeshed geometry. 
		 * The geometry that is to be remeshed can be defined either using an in-memory 
		 * geometry data object, or using a geometry file placed on disk, the latter being 
		 * the preferred method, if the source geometry is very large as it can be swapped 
		 * in/out of memory as needed. 
		 * @return the created object, of type spRemeshingProcessor
		 */
		virtual CountedPointer<IRemeshingProcessor> CreateRemeshingProcessor() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IRemeshingProcessorV2
		 * Documentation from IRemeshingProcessorV2:
		 * IRemeshingProcessorV2 is a newer rewrite of the old IRemeshingProcessor. The 
		 * functionality of the old processor will over time be implemented here and the 
		 * old processor will be replaced. The primary advantages so far are massively 
		 * increased maximum OnScreenSize, much faster processing times for larger sizes, 
		 * more intelligent hole filling algorithms, and optional faster mesh generation 
		 * modes. Memory usage can get pretty extreme at higher sizes depending on input 
		 * mesh, so make sure you have a lot of virtual memory if attempting anything 
		 * >4000. 
		 * @return the created object, of type spRemeshingProcessorV2
		 */
		virtual CountedPointer<IRemeshingProcessorV2> CreateRemeshingProcessorV2() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IScene
		 * Documentation from IScene:
		 * IScene is the main keeper of a scene and scene graph in simplygon. The scene 
		 * is self-contained, with materials, textures, etc. to describe a full scene. 
		 *  
		 * @return the created object, of type spScene
		 */
		virtual CountedPointer<IScene> CreateScene() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISceneNode
		 * Documentation from ISceneNode:
		 * ISceneNode is the base class for all scene graph nodes in simplygon. It has 
		 * relative transformation to its parent, and functions to add children to the 
		 * node. 
		 * @return the created object, of type spSceneNode
		 */
		virtual CountedPointer<ISceneNode> CreateSceneNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISceneNodeCollection
		 * Documentation from ISceneNodeCollection:
		 * ISceneNodeCollection holds a number of ISceneNode objects. The objects can 
		 * be looked up by their names (if they have been named through IObject::SetName() 
		 * ). 
		 * @return the created object, of type spSceneNodeCollection
		 */
		virtual CountedPointer<ISceneNodeCollection> CreateSceneNodeCollection() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISceneMesh
		 * Documentation from ISceneMesh:
		 * ISceneMesh is the mesh node in the simplygon scene graph 
		 * @return the created object, of type spSceneMesh
		 */
		virtual CountedPointer<ISceneMesh> CreateSceneMesh() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISceneLodGroup
		 * Documentation from ISceneLodGroup:
		 * ISceneLodGroup is a LOD grouping for the scene graph 
		 * @return the created object, of type spSceneLodGroup
		 */
		virtual CountedPointer<ISceneLodGroup> CreateSceneLodGroup() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISceneBone
		 * Documentation from ISceneBone:
		 * ISceneBone is the bone node in the simplygon scene graph 
		 * @return the created object, of type spSceneBone
		 */
		virtual CountedPointer<ISceneBone> CreateSceneBone() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISceneBoneTable
		 * Documentation from ISceneBoneTable:
		 * ISceneBoneTable keeps information on bones used while rendering. 
		 * @return the created object, of type spSceneBoneTable
		 */
		virtual CountedPointer<ISceneBoneTable> CreateSceneBoneTable() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IScenePlane
		 * Documentation from IScenePlane:
		 * IScenePlane is the plane node in the simplygon scene graph that can be used 
		 * in various processes, for example as a cutting plane in the RemeshingProcessor. 
		 *  
		 * @return the created object, of type spScenePlane
		 */
		virtual CountedPointer<IScenePlane> CreateScenePlane() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISceneCamera
		 * Documentation from ISceneCamera:
		 * ISceneCamera is the camera node in the Simplygon scene graph. ISceneCamera 
		 * contains settings that specify how the camera works and it contains a number 
		 * of views for that type of camera. A view is a combination of a CameraPosition 
		 * and a TargetPosition. The SceneCamera can contain any number of views but every 
		 * element in TargetPositions has to correspond to a tuple element in CameraPosition. 
		 * TargetPositions is not needed if the camera is set to being omni directional. 
		 *  
		 * @return the created object, of type spSceneCamera
		 */
		virtual CountedPointer<ISceneCamera> CreateSceneCamera() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISelectionSet
		 * Documentation from ISelectionSet:
		 * ISelectionSet is a set of scene node GUIDs referencing scene nodes in a scene. 
		 *  
		 * @return the created object, of type spSelectionSet
		 */
		virtual CountedPointer<ISelectionSet> CreateSelectionSet() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISelectionSetTable
		 * Documentation from ISelectionSetTable:
		 * ISelectionSetTable keeps a table of ISelectionSet. 
		 * @return the created object, of type spSelectionSetTable
		 */
		virtual CountedPointer<ISelectionSetTable> CreateSelectionSetTable() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ITexture
		 * Documentation from ITexture:
		 * IMaterial keeps information on materials used while rendering. 
		 * @return the created object, of type spTexture
		 */
		virtual CountedPointer<ITexture> CreateTexture() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ITextureTable
		 * Documentation from ITextureTable:
		 * ITextureTable keeps information on textures used while rendering. 
		 * @return the created object, of type spTextureTable
		 */
		virtual CountedPointer<ITextureTable> CreateTextureTable() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IImageDataImporter
		 * Documentation from IImageDataImporter:
		 * Class for loading image data from different file formats. 
		 * @return the created object, of type spImageDataImporter
		 */
		virtual CountedPointer<IImageDataImporter> CreateImageDataImporter() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingTextureNode
		 * Documentation from IShadingTextureNode:
		 * ITextureNode describes a texture source node in a shading network. 
		 * @return the created object, of type spShadingTextureNode
		 */
		virtual CountedPointer<IShadingTextureNode> CreateShadingTextureNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingSwizzlingNode
		 * Documentation from IShadingSwizzlingNode:
		 * ISwizzlingNode describes a node that can remap the input to the specified output. 
		 *  
		 * @return the created object, of type spShadingSwizzlingNode
		 */
		virtual CountedPointer<IShadingSwizzlingNode> CreateShadingSwizzlingNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingColorNode
		 * Documentation from IShadingColorNode:
		 * IColorNode describes a single color source node in a shading network 
		 * @return the created object, of type spShadingColorNode
		 */
		virtual CountedPointer<IShadingColorNode> CreateShadingColorNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingAddNode
		 * Documentation from IShadingAddNode:
		 * IAddNode describes a single addition shading node in a shading network. Input 
		 * 1 and 2 are added per-component to produce the output. 
		 * @return the created object, of type spShadingAddNode
		 */
		virtual CountedPointer<IShadingAddNode> CreateShadingAddNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingSubtractNode
		 * Documentation from IShadingSubtractNode:
		 * ISubtractNode describes a single subtracting shading node in a shading network. 
		 *  Input 2 is subtracted from Input 1 per-component. 
		 * @return the created object, of type spShadingSubtractNode
		 */
		virtual CountedPointer<IShadingSubtractNode> CreateShadingSubtractNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingMultiplyNode
		 * Documentation from IShadingMultiplyNode:
		 * IMultiplyNode describes a single multiplying shading node in a shading network. 
		 *  Input 1 and Input 2 is multiplied per-component. 
		 * @return the created object, of type spShadingMultiplyNode
		 */
		virtual CountedPointer<IShadingMultiplyNode> CreateShadingMultiplyNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingPowNode
		 * Documentation from IShadingPowNode:
		 * This node describes an exponentiation. The output will be Input 1 to the power 
		 * of Input 2, and it is calculated per-component. 
		 * @return the created object, of type spShadingPowNode
		 */
		virtual CountedPointer<IShadingPowNode> CreateShadingPowNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingDivideNode
		 * Documentation from IShadingDivideNode:
		 * IDivideNode describes a single division shading node node in a shading network. 
		 *  Input 1 is divided by Input 2 per-component to produce the output. 
		 * @return the created object, of type spShadingDivideNode
		 */
		virtual CountedPointer<IShadingDivideNode> CreateShadingDivideNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingClampNode
		 * Documentation from IShadingClampNode:
		 * IClampNode describes a single clamp shading node in a shading network. Input 
		 * 1 is clamped per-component between Input 2 and 3. 
		 * @return the created object, of type spShadingClampNode
		 */
		virtual CountedPointer<IShadingClampNode> CreateShadingClampNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingMaxNode
		 * Documentation from IShadingMaxNode:
		 * IMaxNode describes a single max shading node in a shading network. The output 
		 * has the per-channel max values of the two inputs. 
		 * @return the created object, of type spShadingMaxNode
		 */
		virtual CountedPointer<IShadingMaxNode> CreateShadingMaxNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingMinNode
		 * Documentation from IShadingMinNode:
		 * IMinNode describes a single min shading node in a shading network. The output 
		 * has the per-channel min values of the two inputs. 
		 * @return the created object, of type spShadingMinNode
		 */
		virtual CountedPointer<IShadingMinNode> CreateShadingMinNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingStepNode
		 * Documentation from IShadingStepNode:
		 * IStepNode describes a single step shading node, where the per-component output 
		 * is either 1 or 0 depending on if input0 is larger or smaller than input1. Basically, 
		 *  (Input0 >= Input1) ? 1 : 0 
		 * @return the created object, of type spShadingStepNode
		 */
		virtual CountedPointer<IShadingStepNode> CreateShadingStepNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingEqualNode
		 * Documentation from IShadingEqualNode:
		 * IShadingEqualNode describes an "is equal to" operator, where the per-component 
		 * output is either 1 or 0 depending on if input0 is equal to input1. Basically, 
		 * (Input0 == Input1) ? 1 : 0 
		 * @return the created object, of type spShadingEqualNode
		 */
		virtual CountedPointer<IShadingEqualNode> CreateShadingEqualNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingNotEqualNode
		 * Documentation from IShadingNotEqualNode:
		 * INotEqualNode describes the "is not equal" operator, where the per-component 
		 * output is either 1 or 0 depending on if input0 is equal to input1. Basically, 
		 * (Input0 != Input1) ? 1 : 0 
		 * @return the created object, of type spShadingNotEqualNode
		 */
		virtual CountedPointer<IShadingNotEqualNode> CreateShadingNotEqualNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingVertexColorNode
		 * Documentation from IShadingVertexColorNode:
		 * IVertexColorNode describes a vertex color source node in a shading network. 
		 *  
		 * @return the created object, of type spShadingVertexColorNode
		 */
		virtual CountedPointer<IShadingVertexColorNode> CreateShadingVertexColorNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingInterpolateNode
		 * Documentation from IShadingInterpolateNode:
		 * IInterpolateNode describes a single interpolating shading node node in a shading 
		 *  network. Input 1 and Input 2 is interpolated per-component using Input 3. 
		 * Blend values below 0 and over 1 will be clamped. 
		 * @return the created object, of type spShadingInterpolateNode
		 */
		virtual CountedPointer<IShadingInterpolateNode> CreateShadingInterpolateNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingCustomNode
		 * Documentation from IShadingCustomNode:
		 * ICustomNode describes a custom shading node in a shading network. The output 
		 * is produced based on customizable user specified operations. The custom node 
		 * has a user specified amount of input parameters. Use SetInputCount() to specify 
		 * the number of inputs. The user has to assign and implement event handlers (called 
		 * observers) for the custom node to be able to evaluate to colors. See CustomNodeExample 
		 * and API documentation for further explanations and code samples. 
		 * @return the created object, of type spShadingCustomNode
		 */
		virtual CountedPointer<IShadingCustomNode> CreateShadingCustomNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingLayeredBlendNode
		 * Documentation from IShadingLayeredBlendNode:
		 * ILayeredBlendNode is a node with an arbitrary number of inputs that can be 
		 * blended differently. 
		 * @return the created object, of type spShadingLayeredBlendNode
		 */
		virtual CountedPointer<IShadingLayeredBlendNode> CreateShadingLayeredBlendNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingNormalize3Node
		 * Documentation from IShadingNormalize3Node:
		 * This node describes an normalization of the x,y,z components of the input. 
		 *  
		 * @return the created object, of type spShadingNormalize3Node
		 */
		virtual CountedPointer<IShadingNormalize3Node> CreateShadingNormalize3Node() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingSqrtNode
		 * Documentation from IShadingSqrtNode:
		 * This node describes a square root calculation. The output will be the per-component 
		 *  square root of input 1 
		 * @return the created object, of type spShadingSqrtNode
		 */
		virtual CountedPointer<IShadingSqrtNode> CreateShadingSqrtNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingDot3Node
		 * Documentation from IShadingDot3Node:
		 * This node describes an Dot of the x,y,z components of the input. 
		 * @return the created object, of type spShadingDot3Node
		 */
		virtual CountedPointer<IShadingDot3Node> CreateShadingDot3Node() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingCross3Node
		 * Documentation from IShadingCross3Node:
		 * This node describes the Cross of the x,y,z components of the input. 
		 * @return the created object, of type spShadingCross3Node
		 */
		virtual CountedPointer<IShadingCross3Node> CreateShadingCross3Node() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingCosNode
		 * Documentation from IShadingCosNode:
		 * This node describes Cos of the x,y,z,w components of the input. 
		 * @return the created object, of type spShadingCosNode
		 */
		virtual CountedPointer<IShadingCosNode> CreateShadingCosNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingSinNode
		 * Documentation from IShadingSinNode:
		 * This node describes Sin of the x,y,z components of the input. 
		 * @return the created object, of type spShadingSinNode
		 */
		virtual CountedPointer<IShadingSinNode> CreateShadingSinNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingGreaterThanNode
		 * Documentation from IShadingGreaterThanNode:
		 * IGreaterThanNode describes the "is greater than" operator, where the per-component 
		 *  output is either 1 or 0 depending on if input0 is greater than input1. Basically, 
		 *  (Input0 > Input1) ? 1 : 0 
		 * @return the created object, of type spShadingGreaterThanNode
		 */
		virtual CountedPointer<IShadingGreaterThanNode> CreateShadingGreaterThanNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadingLessThanNode
		 * Documentation from IShadingLessThanNode:
		 * ILessThanNode describes the "is less than" operator, where the per-component 
		 * output is either 1 or 0 depending on if input0 is LessThan to input1. Basically, 
		 * (Input0 < Input1) ? 1 : 0 
		 * @return the created object, of type spShadingLessThanNode
		 */
		virtual CountedPointer<IShadingLessThanNode> CreateShadingLessThanNode() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShaderGenerator
		 * Documentation from IShaderGenerator:
		 * Given a material, the shader data class keeps the relevant shader data and 
		 * is able to generate a GLSL/HLSL shader. 
		 * @return the created object, of type spShaderGenerator
		 */
		virtual CountedPointer<IShaderGenerator> CreateShaderGenerator() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IAggregationProcessor
		 * Documentation from IAggregationProcessor:
		 * IAggregationProcessor combines all specified geometries in the scene into one 
		 * geometry. All materials are combined and receive a new shared texture atlas. 
		 *  
		 * @return the created object, of type spAggregationProcessor
		 */
		virtual CountedPointer<IAggregationProcessor> CreateAggregationProcessor() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IAggregationSettings
		 * Documentation from IAggregationSettings:
		 * Settings for the IAggregationSettings class. The AggregationProcessor converts 
		 * entire scenes containing multiple draw calls into a single new object with 
		 * a single texture per material channel. The user can set whether or not the 
		 * AggregationProcessor should create new UVs based on the old UVs or create a 
		 * completely new UV parameterization. 
		 * @return the created object, of type spAggregationSettings
		 */
		virtual CountedPointer<IAggregationSettings> CreateAggregationSettings() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IChartAggregator
		 * Documentation from IChartAggregator:
		 * Computes new texture coordinates for a geometry with overlapping UV charts. 
		 *  
		 * @return the created object, of type spChartAggregator
		 */
		virtual CountedPointer<IChartAggregator> CreateChartAggregator() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IVertexColorBaker
		 * Documentation from IVertexColorBaker:
		 * IVertexColorBaker 
		 * @return the created object, of type spVertexColorBaker
		 */
		virtual CountedPointer<IVertexColorBaker> CreateVertexColorBaker() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IImageDataExporter
		 * Documentation from IImageDataExporter:
		 * Class for loading image data from different file formats. 
		 * @return the created object, of type spImageDataExporter
		 */
		virtual CountedPointer<IImageDataExporter> CreateImageDataExporter() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IAmbientOcclusionCaster
		 * Documentation from IAmbientOcclusionCaster:
		 * IAmbientOcclusionCaster generates an ambient occlusion map based on the original 
		 *  geometry, and casts it 
		 * @return the created object, of type spAmbientOcclusionCaster
		 */
		virtual CountedPointer<IAmbientOcclusionCaster> CreateAmbientOcclusionCaster() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IAmbientOcclusionCasterSettings
		 * Documentation from IAmbientOcclusionCasterSettings:
		 * IAmbientOcclusionCasterSettings manages settings for an ambient occlusion caster 
		 *  
		 * @return the created object, of type spAmbientOcclusionCasterSettings
		 */
		virtual CountedPointer<IAmbientOcclusionCasterSettings> CreateAmbientOcclusionCasterSettings() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IImpostorProcessor
		 * Documentation from IImpostorProcessor:
		 * The ImpostorProcessor generates a two-triangle billboard impostor geometry 
		 * of an input geometry from a specific viewing angle, and casts textures and 
		 * normals from the original geometry to the impostor. 
		 * @return the created object, of type spImpostorProcessor
		 */
		virtual CountedPointer<IImpostorProcessor> CreateImpostorProcessor() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IImpostorSettings
		 * Documentation from IImpostorSettings:
		 * Settings for the ImpostorProccessor 
		 * @return the created object, of type spImpostorSettings
		 */
		virtual CountedPointer<IImpostorSettings> CreateImpostorSettings() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ISurfaceMapper
		 * Documentation from ISurfaceMapper:
		 * ISurfaceMapper creates a mapping image between two user-defined geometries. 
		 * The mapper basically looks for the source geometry in the inverse normal direction 
		 * from the outwardly offset destination geometry, and maps pixels accordingly. 
		 * This means that you can modify the normals of the destination geometry to adjust 
		 * the search direction to your liking. The geometries can either be set as IGeometryDatas 
		 * or as the root nodes of scenes. If both are set, the scene is used. 
		 * @return the created object, of type spSurfaceMapper
		 */
		virtual CountedPointer<ISurfaceMapper> CreateSurfaceMapper() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IOcclusionMeshProcessor
		 * Documentation from IOcclusionMeshProcessor:
		 * The occlusion mesh processor creates a reconstruction of the input mesh from 
		 * it's silhouette. This means concavities and internal geometry disappear. WARNING: 
		 * Experimental. Generates nice meshes, but currently very slow at high settings. 
		 * Recommended onscreens size ~100 
		 * @return the created object, of type spOcclusionMeshProcessor
		 */
		virtual CountedPointer<IOcclusionMeshProcessor> CreateOcclusionMeshProcessor() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IFoliageProcessor
		 * @return the created object, of type spFoliageProcessor
		 */
		virtual CountedPointer<IFoliageProcessor> CreateFoliageProcessor() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IShadowMeshProcessor
		 * Documentation from IShadowMeshProcessor:
		 * The shadow mesh processor creates shadow meshes for cheap shadow mapping, either 
		 *  from a specific direction or viewable from anywhere WARNING: Experimental. 
		 * Inconsistent results, and currently very slow at high settings. Recommended 
		 * onscreensize ~100. 
		 * @return the created object, of type spShadowMeshProcessor
		 */
		virtual CountedPointer<IShadowMeshProcessor> CreateShadowMeshProcessor() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IImageComparer
		 * Documentation from IImageComparer:
		 * compares images 
		 * @return the created object, of type spImageComparer
		 */
		virtual CountedPointer<IImageComparer> CreateImageComparer() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IRecommendedTextureSizeCalculator
		 * Documentation from IRecommendedTextureSizeCalculator:
		 * Estimates a coarse recommended UV size (width and height) for a geometry based 
		 * on only its surface area and either the distance it is allowed to deviate or 
		 * its intended on screen size. The UVs are assumed to be unique i.e. not tiled/repeated. 
		 * RecommendedWidth and RecommendedHeight will only differ from each other if 
		 * ForcePower2Texture is enabled. 
		 * @return the created object, of type spRecommendedTextureSizeCalculator
		 */
		virtual CountedPointer<IRecommendedTextureSizeCalculator> CreateRecommendedTextureSizeCalculator() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IPartRemover
		 * Documentation from IPartRemover:
		 * The PartRemover tool can identify unconnected sub-meshes, and remove meshes 
		 * that fall below a set size threshold. This size threshold can be set globally 
		 * for the entire geometry, or per-material to be able to remove things like decals 
		 * effectively. 
		 * @return the created object, of type spPartRemover
		 */
		virtual CountedPointer<IPartRemover> CreatePartRemover() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IWelder
		 * Documentation from IWelder:
		 * welds geometry vertices 
		 * @return the created object, of type spWelder
		 */
		virtual CountedPointer<IWelder> CreateWelder() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type ITJunctionEliminator
		 * Documentation from ITJunctionEliminator:
		 * Removes t-junctions by subdividing triangles and welding vertices 
		 * @return the created object, of type spTJunctionEliminator
		 */
		virtual CountedPointer<ITJunctionEliminator> CreateTJunctionEliminator() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IPipelineSerializer
		 * @return the created object, of type spPipelineSerializer
		 */
		virtual CountedPointer<IPipelineSerializer> CreatePipelineSerializer() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IReductionPipeline
		 * Documentation from IReductionPipeline:
		 * IReductionPipeline provides a pipeline for a single reduction processor to 
		 * be applied on the input scene. 
		 * @return the created object, of type spReductionPipeline
		 */
		virtual CountedPointer<IReductionPipeline> CreateReductionPipeline() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IRemeshingPipeline
		 * Documentation from IRemeshingPipeline:
		 * IRemeshingPipeline provides a pipeline for a single remeshing processor to 
		 * be applied on the input scene. 
		 * @return the created object, of type spRemeshingPipeline
		 */
		virtual CountedPointer<IRemeshingPipeline> CreateRemeshingPipeline() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IAggregationPipeline
		 * Documentation from IAggregationPipeline:
		 * IAggregationPipeline provides a pipeline for a single aggregation processor 
		 * to be applied on the input scene. 
		 * @return the created object, of type spAggregationPipeline
		 */
		virtual CountedPointer<IAggregationPipeline> CreateAggregationPipeline() = 0;

		/**
		 * Creates and returns a counted pointer to an object of type IPipelineSettings
		 * Documentation from IPipelineSettings:
		 * IPipelineSettings is the interface used to manage settings for a pipeline 
		 * @return the created object, of type spPipelineSettings
		 */
		virtual CountedPointer<IPipelineSettings> CreatePipelineSettings() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IBoolData interface
		 * @return the created object
		 */
		virtual rdata<IBoolData> CreateBoolData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an ICharData interface
		 * @return the created object
		 */
		virtual rdata<ICharData> CreateCharData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IUnsignedCharData interface
		 * @return the created object
		 */
		virtual rdata<IUnsignedCharData> CreateUnsignedCharData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IShortData interface
		 * @return the created object
		 */
		virtual rdata<IShortData> CreateShortData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IUnsignedShortData interface
		 * @return the created object
		 */
		virtual rdata<IUnsignedShortData> CreateUnsignedShortData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IIntData interface
		 * @return the created object
		 */
		virtual rdata<IIntData> CreateIntData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IUnsignedIntData interface
		 * @return the created object
		 */
		virtual rdata<IUnsignedIntData> CreateUnsignedIntData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an ILongData interface
		 * @return the created object
		 */
		virtual rdata<ILongData> CreateLongData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IUnsignedLongData interface
		 * @return the created object
		 */
		virtual rdata<IUnsignedLongData> CreateUnsignedLongData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IFloatData interface
		 * @return the created object
		 */
		virtual rdata<IFloatData> CreateFloatData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IDoubleData interface
		 * @return the created object
		 */
		virtual rdata<IDoubleData> CreateDoubleData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IRealData interface
		 * @return the created object
		 */
		virtual rdata<IRealData> CreateRealData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IRidData interface
		 * @return the created object
		 */
		virtual rdata<IRidData> CreateRidData() = 0;

		/**
		 * Creates and returns an rdata counted pointer to an IRhandleData interface
		 * @return the created object
		 */
		virtual rdata<IRhandleData> CreateRhandleData() = 0;

		/**
		 * Sets the callback used to report errors
		 * @param handler is the new error handler object
		 * @return the previously registered handler object
		 */
		virtual rerrorhandler *SetErrorHandler( rerrorhandler *handler ) = 0;
		/**
		 * Returns the version of the Simplygon library as a string
		 * @return the library version
		 */
		virtual const char *GetVersion() const = 0;
		/**
		 * Sets a named global library setting
		  * @param setting_name is the name of the global setting we want to set.
		 * @param value is the value we want to set the chosen global setting to.
		 */
		virtual void SetGlobalSetting( const char *setting_name , intptr_t value ) const = 0;
		/**
		 * Gets a named global library setting
		  * @param setting_name is the name of the global setting we want to get.
		 * @return the value of the chosen setting.
		 */
		virtual intptr_t GetGlobalSetting( const char *setting_name ) const = 0;
		/**
		 * Sets a named library setting that is local to the current thread
		  * @param setting_name is the name of the setting we want to set.
		 * @param value is the value we want to set the chosen setting to.
		 */
		virtual void SetThreadLocalSetting( const char *setting_name , intptr_t value ) const = 0;
		/**
		 * Gets a named library setting that is local to the current thread
		  * @param setting_name is the name of the setting we want to get.
		 * @return the value of the chosen setting.
		 */
		virtual intptr_t GetThreadLocalSetting( const char *setting_name ) const = 0;
	};

    /**
     * SafeCast casts a pointer to an API object into a pointer to another API class type. The cast is only 
     * made if the object is of the type it is cast into, else NULL is returned.
     * @param ptr is the pointer that is to be cast
     * @return a counted pointer of the template T cast type 
     */
    template<class T> CountedPointer<T> SafeCast( IObject *ptr )
        {
        return CountedPointer<T>(T::SafeCast(ptr));
        }
    
    /**
     * UnsafeCast casts a pointer to an API object into a pointer to another API class type. The pointer is cast
     * using static_cast, and is done regardless of if the object is actually of the type it is cast into
     * @param ptr is the pointer that is to be cast
     * @return a counted pointer of the template T cast type 
     */
    template<class T> CountedPointer<T> UnsafeCast( IObject *ptr )
        {
        return CountedPointer<T>(static_cast<T*>(ptr));
        }
    
    /**
     * Cast casts a pointer to an API object into a pointer to another API class type. The cast is only 
     * made if the object is of the type it is cast into, else NULL is returned.
     * @param ptr is the pointer that is to be cast
     * @return a counted pointer of the template T cast type 
     */
    template<class T> CountedPointer<T> Cast( IObject *ptr )
        {
        return SafeCast<T>(ptr);
        }

	/**
	 * Returns the API interface version hash as a string
	 * @return the API interface version hash
	 */
	inline const char *GetInterfaceVersionHash()
		{
		return "BC34FC7A54A4BE733345BF339D32C8F0E6881FC2A340054CDB5A2262CDDEBFA7";
		}

	/**
	 * Returns the SDK version number of the header file as a string. This may not match the .dll, because headers and .dll files that have the same interface will work properly.
	 * @return the API version string
	 */
	inline const char *GetHeaderVersion()
		{
		return "8.3.37100";
		}

};

#ifdef _WIN32
#pragma warning( pop )
#endif
#endif//_SIMPLYGONSDK_H_
