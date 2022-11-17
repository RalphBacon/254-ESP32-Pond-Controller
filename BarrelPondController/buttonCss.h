#pragma once

const char *buttonCSS = R"(
    .button {
		position: relative;
		font-size: 28px;
		color: #FFFFFF;
		padding: 20px;
		width: 200px;
		text-align: center;
		transition-duration: 0.4s;
		text-decoration: none;
		overflow: hidden;
		cursor: pointer;
		border-radius: 12px;
		}
	.turnON {
		background-color: #4CAF50;
		border: 1px solid forestgreen;
	}
	.turnOFF {
		background-color: red;
		border: 1px solid darkred;
	}

	.footer {
		position: absolute;
		bottom: 50px;
		margin-left: auto;
  		margin-right: auto;
		width:98%%;
	}

	table {
		margin-left: auto;
  		margin-right: auto;
		margin-top: 20px;
		width:75%%;
	}

	table, th, td {
  		border: 1px solid black;
		border-collapse: collapse;
		}

	tr:nth-child(odd) {
		background-color: rgba(204,204,204,0.6);
	}

	tr {
		height: 35px;
	}

	th {
		background-color: rgba(157,157,157,0.8);
	}
)";