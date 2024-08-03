import threading


def print_cube(num):
	while True:
		print("Cube: {}" .format(num * num * num))


def print_square(num):
	while True:
		print("Square: {}" .format(num * num))


def main():
	t1 = threading.Thread(target=print_square, args=(10,))
	t2 = threading.Thread(target=print_cube, args=(10,))

	t1.start()
	t2.start()

	t1.join()
	t2.join()

	print("Done!")

if __name__ =="__main__":
	main()
