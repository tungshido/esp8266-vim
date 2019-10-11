const { poolPromise } = require('./createPoolConnection');
module.exports = {
  dataQuery: async (machineId, startTime, endTime) => {
    try {
      const pool = await poolPromise;
      /*let result = await pool.query(`SELECT TOP (3000) [ID]
      ,[CreationTime]
      ,[Machine]
      ,[Data]
  FROM [IoTdata].[dbo].[DT_CycleTime]`); */
      let result = await pool.query(
        `SELECT [Data] FROM [IoTdata].[dbo].[DT_CycleTime] WHERE [CreationTime] 
        between '${startTime}' and '${endTime}' and [Machine] = '${machineId}'`,
      );
      console.log(result['recordsets'][0]);
      return result['recordsets'][0];
    } catch (error) {
      console.log(error);
    }
  },
};
