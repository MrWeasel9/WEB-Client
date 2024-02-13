Sure, here's the README formatted in Markdown for GitHub:

# Library Management System

This project is a library management system implemented in C, utilizing the Parson library for JSON data manipulation. Parson library was chosen for its capabilities in parsing and generating JSON structures within the C programming language. It is used to analyze a JSON string and transform it into an internal structure of JSON objects, which are then sent to the server in JSON format.

## Features

- User authentication using cookies and JWT token
- Registration, login, and logout functionalities
- Access control to library resources
- Adding, viewing, and deleting books

## Dependencies

- Parson library for JSON manipulation

## Usage

### Constants

Define the IP and PORT constants for server connection.

```c
#define IP "127.0.0.1"
#define PORT 8080
```

### API Routes

Define the routes for REST API endpoints:

- REGISTER
- LOGIN
- LOGOUT
- ENTER
- BOOKS

### Helper Function

Create a helper function to validate whether a string represents a valid integer. This function is used to verify if the IDs entered by the user are numbers.

### Variables

- `message`: Variable for sending data to the server.
- `response`: Variable for receiving data from the server.
- `cookies`: Array to store authentication cookies.
- `token`: JWT token.
- `connected`: Semaphore indicating whether the user is logged in or not.
- `sockfd`: Socket for server connection.

### Register and Login

Build POST requests to send registration/authentication information to the server. Save the cookie for login in the `cookies` array and change the `connected` semaphore.

### Enter Library

Check if the user is authenticated. If yes, build a GET request and extract the JWT token received from the server.

### Get Books and Get Book

Check if the user is logged in and has access to the library. If everything is fine, construct a GET request to retrieve the books.

### Add Book

Get the title, author, genre, publisher, and number of pages of the book and build a POST request to add the book.

### Delete Book

Get the ID of the book to be deleted and construct a DELETE request to remove the book.

### Logout

Send a POST request to perform the logout operation.

### Reconnecting

Reopen the connection for each command using the `open_connection` command.

## License

This project is licensed under the [MIT License](LICENSE).
