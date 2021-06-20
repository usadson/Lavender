# Code Style
The code style is very similar to the [WebKit Code Style Guidelines](https://webkit.org/code-style-guidelines/), except
for a few minor differences.

## Switch
The case-labels of the switch statement *are* indented:
```cpp
switch (integer) {
	case 0: return ZERO;
	case 1: return ONE;
	default: return UNKNOWN;
}
```

## Function Style
The function definition and declaration style is also somewhat different. It is more similar to the BSD style:
```cpp
[[nodiscard]] inline constexpr const auto &
ns::clss::functionName(auto &thisIsAKindaLongInputName,
					   const auto &input) noexcept {
	static_cast<void>(thisIsAKindaLongInputName);
	return input * 2;
}
```

1. Everything before the function name specifier is put on a separate line.
2. The opening bracket isn't on a separate line.

## Column Limit
As far as I know, WebKit doesn't specify a column limit. In this project, a column limit of 120 is used.

## Namespace Indentation
Namespaces blocks are indented.
```cpp
namespace example {

	void test() {

	}

} // namespace example
```
