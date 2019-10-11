const createError = require('http-errors');
const express = require('express');
const path = require('path');
const cookieParser = require('cookie-parser');
const logger = require('morgan');
const sql = require('mssql');
const ip = require('ip');
const indexRouter = require('./routes/index');
const usersRouter = require('./routes/users');
const getDataRouter = require('./routes/data');

const request = require('request');
const bodyParser = require('body-parser');
const app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

app.use(logger('dev'));
app.use(express.json());
app.use(
  express.urlencoded({
    extended: false,
  }),
);

app.use(bodyParser.json());
app.use(
  bodyParser.urlencoded({
    extended: true,
  }),
);

app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

//==================Routes====================//
app.use('/', indexRouter);
app.use('/users', usersRouter);
app.use('/data', getDataRouter);

//=================Database connection===============//
//const sqlConnection = require('./api/sqlConnect.js');

// catch 404 and forward to error handler
app.use((req, res, next) => {
  next(createError(404));
});
app.post((req, res, next) => {
  console.log(req.body.user.name);
});
app.get((req, res, next) => {
  next();
});

// error handler
app.use((err, req, res, next) => {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = {
  app: app,
  sql: sql,
  request: request,
  ip: ip,
};
