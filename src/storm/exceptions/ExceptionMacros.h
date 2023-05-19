#ifndef STORM_EXCEPTIONS_EXCEPTIONMACROS_H_
#define STORM_EXCEPTIONS_EXCEPTIONMACROS_H_

/*!
 * Macro to generate descendant exception classes. As all classes are nearly the same, this makes changing common
 * features much easier.
 */
#define STORM_NEW_EXCEPTION(exception_name)                             \
    class exception_name : public BaseException {                       \
       public:                                                          \
        exception_name() : BaseException() {}                           \
        exception_name(char const* cstr) : BaseException(cstr) {}       \
        exception_name(exception_name const& cp) : BaseException(cp) {} \
        ~exception_name() throw() {}                                    \
        virtual std::string type() const override {                     \
            return #exception_name;                                     \
        }                                                               \
        template<typename T>                                            \
        exception_name& operator<<(T const& var) {                      \
            this->stream << var;                                        \
            return *this;                                               \
        }                                                               \
    };

#endif /* STORM_EXCEPTIONS_EXCEPTIONMACROS_H_ */
