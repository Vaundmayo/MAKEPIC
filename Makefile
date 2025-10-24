# 사용할 컴파일러 (과제 요구 사항에 따라 gcc 명시)
CC = gcc

# 컴파일 경고 옵션 (모든 경고 활성화)
CFLAGS = -Wall

# 링커 플래그
LDFLAGS = 

# 컴파일할 소스 파일
SRCS = MAKEPIC_test.c

# 생성할 실행 파일 이름
TARGET = makepic

# 'make' 또는 'make all' 명령어로 실행 파일을 생성
all: $(TARGET)

# 실행 파일을 생성하는 규칙
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# 'make run' 명령어로 컴파일 후 즉시 실행
run: all
	./$(TARGET)

# 'make clean' 명령어로 컴파일된 파일을 삭제
clean:
	rm -f $(TARGET)