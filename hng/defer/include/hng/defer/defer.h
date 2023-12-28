#ifndef HNG_DEFER_HEADERGUARD
#define HNG_DEFER_HEADERGUARD
//
//	Author:		Elijah Shadbolt
//	Date:		28 Dec 2023
//	Licence:	MIT
//	GitHub:		https://github.com/highestnamegames/defer
//	Version:	v1.1.0
//
//	Summary:
//		Defers execution of statements to the end of the scope.
//		Compatible with C++11, C++14, C++17.
//
//	Tips:
//		Use `std::unique_ptr` with custom deleter to return
//		RAII-wrapped non-pointer types from functions.
//

#include <exception>
#include <utility>
#include <type_traits>
#include <stdexcept>

#if ((__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1913)))
#define DETAIL_HNG_DEFER_HAS_CPP17 1
#else
#define DETAIL_HNG_DEFER_HAS_CPP17 0
#endif

#if DETAIL_HNG_DEFER_HAS_CPP17
#define DETAIL_HNG_DEFER_CONSTEXPR_VAR constexpr
#else
#define DETAIL_HNG_DEFER_CONSTEXPR_VAR
#endif

#if DETAIL_HNG_DEFER_HAS_CPP17
#define DETAIL_HNG_DEFER_CONSTEXPR_IF constexpr
#else
#define DETAIL_HNG_DEFER_CONSTEXPR_IF
#endif

namespace hng {
	template<class Callable>
	#if (defined(__cpp_concepts) && __cpp_concepts >= 201907L)
	requires noexcept(std::declval<Callable&&>()())
	#endif
	struct defer
	{
	private:
		Callable m_callable;
	public:
		inline ~defer() noexcept { std::move(m_callable)(); }
		inline defer(defer const&) = delete;
		inline defer(defer&&) = delete;
		inline defer& operator=(defer const&) = delete;
		inline defer& operator=(defer&&) = delete;
		inline defer() noexcept(std::is_nothrow_default_constructible_v<Callable>) : m_callable() {}
		inline explicit defer(Callable&& callable) noexcept(std::is_nothrow_move_constructible_v<Callable>) : m_callable(std::move(callable)) {}
		inline explicit defer(Callable const& callable) noexcept(std::is_nothrow_copy_constructible_v<Callable>) : m_callable(callable) {}
	};


	class defer_exception : public std::runtime_error {
	private:
		std::exception_ptr m_exception_ptr{};
	public:
		inline virtual ~defer_exception() noexcept = default;
		inline defer_exception(defer_exception&&) noexcept = default;
		inline defer_exception(defer_exception const&) = default;
		inline defer_exception& operator=(defer_exception&&) noexcept = default;
		inline defer_exception& operator=(defer_exception const&) = default;
		inline defer_exception() : runtime_error("defer exception") {}
		inline explicit defer_exception(char const* message, std::exception_ptr&& exception_ptr) : runtime_error(message), m_exception_ptr(std::move(exception_ptr)) {}
		inline explicit defer_exception(char const* message, std::exception_ptr const& exception_ptr) : runtime_error(message), m_exception_ptr(exception_ptr) {}
		inline explicit defer_exception(std::string const& message, std::exception_ptr&& exception_ptr) : runtime_error(message), m_exception_ptr(std::move(exception_ptr)) {}
		inline explicit defer_exception(std::string const& message, std::exception_ptr const& exception_ptr) : runtime_error(message), m_exception_ptr(exception_ptr) {}
		inline explicit defer_exception(std::exception_ptr&& exception_ptr) : defer_exception("defer exception", std::move(exception_ptr)) {}
		inline explicit defer_exception(std::exception_ptr const& exception_ptr) : defer_exception("defer exception", exception_ptr) {}
		inline std::exception_ptr& exception_ptr() noexcept { return m_exception_ptr; }
		inline std::exception_ptr const& exception_ptr() const noexcept { return m_exception_ptr; }
	};

	namespace detail {
		namespace defer {

			template<class F, typename EIF = std::enable_if_t<std::is_same_v<std::decay_t<decltype(std::declval<F&&>()())>, void>, void>>
			int dt_invoke(F&& f) {
				std::forward<F>(f)();
				return 0;
			}

			template<class F, typename EIF = std::enable_if_t<!std::is_same_v<std::decay_t<decltype(std::declval<F&&>()())>, void>, void>>
			decltype(auto) dt_invoke(F&& f) {
				return std::forward<F>(f)();
			}

			template<class T, typename EIF = std::enable_if_t<std::is_lvalue_reference_v<T>, void>>
			T dt_return_forward(T value) noexcept {
				return value;
			}

			template<class T, class U, typename EIF = std::enable_if_t<!std::is_lvalue_reference_v<T>, void>>
			std::remove_reference_t<T> dt_return_forward(U&& value) noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, U&&>) {
				return T(std::move(value));
			}

		}
	}
}


#define DETAIL_HNG_DT_DEFER_FINALLY(mode)\
	(([&]()->decltype(auto){\
	DETAIL_HNG_DEFER_CONSTEXPR_VAR int const DETAIL_HNG_DT_mode=(mode);\
	bool DETAIL_HNG_DT_invoked=0;\
	auto DETAIL_HNG_DT_finally=(


// If the try block throws and the defer block throws, the defer block exception is propagated and the try block exception is lost.
#define HNG_DT_DEFER_FINALLY\
	DETAIL_HNG_DT_DEFER_FINALLY(0)


// If the try block throws and the defer block throws, the defer block exception is propagated via std::rethrow_exception()
//   as a hng::defer_exception (which contains the exception_ptr that the defer block threw)
//   mixed in with a std::nested_exception (the nested exception is the try block exception).
// If the try block does not throw and the defer block throws, the defer block exception is propagated as-is.
#define HNG_DT_DEFER_FINALLY_PRESERVE\
	DETAIL_HNG_DT_DEFER_FINALLY(1)


#define HNG_DT_TRY\
	);try{auto DETAIL_HNG_DT_try_fn=(


#define DETAIL_HNG_DT_END_INVOKE_FINALLY(trycaught)\
	do{\
		_Pragma("warning(push)")\
		_Pragma("warning(disable : 4127)")\
		if DETAIL_HNG_DEFER_CONSTEXPR_IF ((trycaught)&&1==DETAIL_HNG_DT_mode){\
		_Pragma("warning(pop)")\
			::std::exception_ptr DETAIL_HNG_DT_try_block_exception=::std::current_exception();\
			try{\
				::std::move(DETAIL_HNG_DT_finally)();\
			}catch(...){\
				::std::exception_ptr DETAIL_HNG_DT_defer_block_exception=::std::current_exception();\
				try{\
					::std::rethrow_exception(::std::move(DETAIL_HNG_DT_try_block_exception));\
				}catch(...){\
					::std::throw_with_nested(::hng::defer_exception(::std::move(DETAIL_HNG_DT_defer_block_exception)));\
				}\
			}\
		}\
		else{::std::move(DETAIL_HNG_DT_finally)();}\
	}while(0)


#if DETAIL_HNG_DEFER_HAS_CPP17

#define DETAIL_HNG_DT_END_TRY_STATEMENTS\
	do{\
		using DETAIL_HNG_DT_try_fn_result_t=decltype(::std::move(DETAIL_HNG_DT_try_fn)());\
		if constexpr(::std::is_same_v<::std::decay_t<DETAIL_HNG_DT_try_fn_result_t>,void>){\
			::std::move(DETAIL_HNG_DT_try_fn)();\
			DETAIL_HNG_DT_invoked=1;\
			DETAIL_HNG_DT_END_INVOKE_FINALLY(0);\
		}else if constexpr(::std::is_lvalue_reference_v<DETAIL_HNG_DT_try_fn_result_t>){\
			DETAIL_HNG_DT_try_fn_result_t DETAIL_HNG_DT_result=::std::move(DETAIL_HNG_DT_try_fn)();\
			DETAIL_HNG_DT_invoked=1;\
			DETAIL_HNG_DT_END_INVOKE_FINALLY(0);\
			return DETAIL_HNG_DT_result;\
		}else{\
			::std::decay_t<DETAIL_HNG_DT_try_fn_result_t>DETAIL_HNG_DT_result=::std::move(DETAIL_HNG_DT_try_fn)();\
			DETAIL_HNG_DT_invoked=1;\
			DETAIL_HNG_DT_END_INVOKE_FINALLY(0);\
			return DETAIL_HNG_DT_result;\
		}\
	}while(0)

#else // ^^^^ DETAIL_HNG_DEFER_HAS_CPP17

#define DETAIL_HNG_DT_END_TRY_STATEMENTS\
	do{\
		using DETAIL_HNG_DT_try_fn_result_t=decltype(::std::move(DETAIL_HNG_DT_try_fn)());\
		::std::conditional_t<\
			::std::is_same_v<::std::decay_t<DETAIL_HNG_DT_try_fn_result_t>,void>,\
			int,\
			::std::conditional_t<\
			::std::is_lvalue_reference_v<DETAIL_HNG_DT_try_fn_result_t>,\
			DETAIL_HNG_DT_try_fn_result_t,\
			::std::decay_t<DETAIL_HNG_DT_try_fn_result_t>\
		>>DETAIL_HNG_DT_result=::hng::detail::defer::dt_invoke(::std::move(DETAIL_HNG_DT_try_fn));\
		DETAIL_HNG_DT_invoked=1;\
		DETAIL_HNG_DT_END_INVOKE_FINALLY(0);\
		return ::hng::detail::defer::dt_return_forward<DETAIL_HNG_DT_try_fn_result_t>(DETAIL_HNG_DT_result);\
	}while(0)

#endif // ^^^^ !DETAIL_HNG_DEFER_HAS_CPP17


#define HNG_DT_END\
		);\
		DETAIL_HNG_DT_END_TRY_STATEMENTS;\
	}catch(...){\
		if(!DETAIL_HNG_DT_invoked){\
			DETAIL_HNG_DT_END_INVOKE_FINALLY(1);\
		}\
		throw;\
	}\
	})())




#define DETAIL_HNG_DEFER_CAT_IMPL(a, b) a ## b
#define DETAIL_HNG_DEFER_CAT(a, b) DETAIL_HNG_DEFER_CAT_IMPL(a, b)
#define DETAIL_HNG_DEFER_LINEGENNAME DETAIL_HNG_DEFER_CAT(line, __LINE__)


/*	Example:
	```
		int* raw_ptr = new int(5);
		HNG_DEFER_NAMED_BEGIN(random_name)
		{
			delete raw_ptr;
			raw_ptr = nullptr;
		}
		HNG_DEFER_NAMED_END(random_name);
		// ... do stuff with raw_ptr ...
	```
*/
#define HNG_DEFER_NAMED_BEGIN(name)\
	auto DETAIL_HNG_DEFER_CAT(HNG_DEFER_fn_,name)=[&]()noexcept{


#define HNG_DEFER_NAMED_END(name)\
	};::hng::defer<decltype(DETAIL_HNG_DEFER_CAT(HNG_DEFER_fn_,name))>DETAIL_HNG_DEFER_CAT(HNG_DEFER_var_,name)(::std::move(DETAIL_HNG_DEFER_CAT(HNG_DEFER_fn_,name)));do{}while(0)


#define HNG_DEFER_BLOCK(...)\
	HNG_DEFER_NAMED_BEGIN(DETAIL_HNG_DEFER_LINEGENNAME){__VA_ARGS__}HNG_DEFER_NAMED_END(DETAIL_HNG_DEFER_LINEGENNAME)


#define HNG_DEFER_CALLABLE_VARIABLE(callable_variable)\
	hng::defer<decltype(callable_variable)> const DETAIL_HNG_DEFER_CAT(HNG_DEFER_var_,DETAIL_HNG_DEFER_LINEGENNAME) (::std::move(callable_variable));



#if DETAIL_HNG_DEFER_HAS_CPP17


#define HNG_DEFER_BEGIN\
	auto const DETAIL_HNG_DEFER_CAT(HNG_DEFER_var_,DETAIL_HNG_DEFER_LINEGENNAME)=::hng::defer([&]()noexcept{


#define HNG_DEFER_END\
	});do{}while(0)


#endif // ^^^ DETAIL_HNG_DEFER_HAS_CPP17


#endif // ^^^ HNG_DEFER_HEADERGUARD
