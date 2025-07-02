#include <linux/module.h>  // Базовые макросы для модулей ядра
#include <linux/fs.h>      // Работа с файловой системой и file_operations
#include <linux/cdev.h>    // Для работы с символьными устройствами (struct cdev)
#include <linux/uaccess.h> // Функции доступа к пользовательской памяти
#include <linux/device.h>  // Управление устройствами (device_create, class_create)

// Имя устройства. Будет создано как /dev/Hello
#define DEVICE_NAME "Hello"
// Размер буфера сообщений (в байтах)
#define BUF_LEN 1024

// Переменная для хранения номера устройства (major + minor)
static dev_t dev_num;

// Структура символьного устройства
static struct cdev char_dev;

// Класс устройства — нужен для автоматического создания файла в /dev
static struct class *dev_class;

// Ядерный буфер, в который будут записываться данные из пользовательского пространства
static char msg[BUF_LEN] = {0};

// Глобальная переменная, указывающая на текущую позицию в буфере при чтении/записи
// ❗ Не потокобезопасно! Лучше использовать loff_t *offset.
static int pos = 0;

/**
 * device_open - Вызывается при открытии устройства
 * @inode: inode-структура, связанная с файлом
 * @file: файловая структура, представляющая открытое устройство
 *
 * Эта функция вызывается, когда процесс открывает устройство через open("/dev/Hello", ...)
 */
static int device_open(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "Device opened\n"); // Выводим сообщение в лог ядра
  return 0;                            // Возвращаем успех
}

/**
 * device_read - Вызывается при чтении данных из устройства
 * @file: файловая структура
 * @buffer: указатель на буфер в пользовательском пространстве, куда будут записаны данные
 * @length: количество запрошенных байтов
 * @offset: текущее смещение в файле (указатель на позицию чтения)
 *
 * Эта функция копирует данные из ядерного буфера `msg` в пользовательский буфер `buffer`.
 * После успешного чтения обновляется значение `*offset`.
 */
static ssize_t device_read(struct file *file, char __user *buffer, size_t length, loff_t *offset)
{
  size_t remaining = BUF_LEN - *offset; // Сколько осталось прочитать

  // Если достигнут конец буфера или не требуется читать — возвращаем 0 (EOF)
  if (*offset >= BUF_LEN || !length)
    return 0;

  // Ограничиваем длину чтения оставшимся объёмом
  if (length > remaining)
    length = remaining;

  // Копируем данные из ядра в пользовательское пространство
  if (copy_to_user(buffer, msg + *offset, length))
    return -EFAULT; // Ошибка доступа к пользовательской памяти

  *offset += length; // Обновляем смещение
  return length;     // Возвращаем количество успешно прочитанных байт
}

/**
 * device_write - Вызывается при записи данных в устройство
 * @file: файловая структура
 * @buffer: указатель на буфер в пользовательском пространстве, откуда будут взяты данные
 * @length: количество байтов, которые нужно записать
 * @offset: текущее смещение в файле
 *
 * Эта функция копирует данные из пользовательского буфера `buffer` в ядерный буфер `msg`.
 * Запись происходит по одному байту через get_user().
 */
static ssize_t device_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset)
{
  int i;

  pos = 0; // Сбрасываем позицию перед новой записью

  // Побайтово копируем данные из пользовательского буфера в ядерный
  for (i = 0; i < length && i < BUF_LEN - 1; i++)
  {
    get_user(msg[i], &buffer[i]); // Читаем один байт из пользовательского буфера
    pos++;                        // Увеличиваем позицию
  }

  msg[pos] = '\0'; // Добавляем завершающий ноль, чтобы строка была корректной

  printk(KERN_INFO "Received: %s\n", msg); // Выводим полученные данные в лог ядра
  return i;                                // Возвращаем количество записанных байт
}

/**
 * device_release - Вызывается при закрытии устройства
 * @inode: inode-структура
 * @file: файловая структура
 *
 * Эта функция вызывается, когда процесс закрывает устройство через close()
 */
static int device_release(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "Device closed\n"); // Выводим сообщение в лог ядра
  return 0;                            // Возвращаем успех
}

/**
 * file_operations — таблица, связывающая системные вызовы с нашими функциями
 * Например:
 * - read() → device_read
 * - write() → device_write
 */
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

/**
 * driver_init - Функция инициализации модуля
 * Вызывается при загрузке модуля в ядро (insmod)
 */
static int __init driver_init(void)
{
  alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME); // Выделяем диапазон major/minor номеров
  cdev_init(&char_dev, &fops);                      // Инициализируем символьное устройство
  cdev_add(&char_dev, dev_num, 1);                  // Регистрируем устройство в системе

  dev_class = class_create(THIS_MODULE, DEVICE_NAME);         // Создаём класс устройства
  device_create(dev_class, NULL, dev_num, NULL, DEVICE_NAME); // Создаём файл устройства в /dev

  printk(KERN_INFO "Char driver is opened"); // Сообщение в лог ядра
  return 0;
}

/**
 * driver_exit - Функция очистки модуля
 * Вызывается при выгрузке модуля из ядра (rmmod)
 */
static void __exit driver_exit(void)
{
  cdev_del(&char_dev);                       // Удаляем символьное устройство
  unregister_chrdev_region(dev_num, 1);      // Освобождаем номер устройства
  device_destroy(dev_class, dev_num);        // Удаляем файл устройства
  class_destroy(dev_class);                  // Удаляем класс устройства
  printk(KERN_INFO "Goodbye Char Driver\n"); // Сообщение в лог ядра
}

// Указываем, какие функции вызывать при загрузке и выгрузке
module_init(driver_init);
module_exit(driver_exit);

// Информация о модуле — отображается в modinfo
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vladislav");
MODULE_DESCRIPTION("Simple Character Driver");