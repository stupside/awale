#define SET_COLOR(code) printf("\033[%sm", code)
#define RESET_COLOR() printf("\033[0m")
#define COLOR_BLUE "1;34"   // Bold Blue
#define COLOR_GREEN "1;32"  // Bold Green
#define COLOR_RED "1;31"    // Bold Red
#define COLOR_YELLOW "1;33" // Bold Yellow
#define COLOR_CYAN "1;36"   // Bold Cyan
#define COLOR_PURPLE "1;35" // Bold Purple

#define PRINT_COLOR(color, fmt, ...)                                           \
  SET_COLOR(color);                                                            \
  printf(fmt, ##__VA_ARGS__);                                                  \
  RESET_COLOR()