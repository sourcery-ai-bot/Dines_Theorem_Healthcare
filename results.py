#import pandas as pd 
import pickle
import numpy as np
import pandas as pd 
import scipy.optimize as opt
import random
import csv
import re
import sys

'''initialize variables'''

country_file = "country_health_data.txt"

''' LISTS '''

democracies = [ # democracies determined by countries with at least a democracy score of 6 according to https://worldpopulationreview.com/countries/democracy-countries/
    "Argentina",
    "Australia",
    "Austria",
    "Belgium",
#    "Botswana",
    "Brazil",
    "Bulgaria",
    "Canada",
#    "Cape Verde",
#    "Chile",
#    "Colombia",
#    "Costa Rica",
    "Croatia",
    "Cyprus",
    "Czechia",
    "Denmark",
#    "Dominican Republic",
#    "Ecuador",
    "Estonia",
    "Finland",
    "France",
    "Germany",
#    "Ghana",
    "Greece",
#    "Hong Kong",
    "Hungary",
    "Iceland",
#    "India",
#    "Indonesia",
    "Ireland",
    "Israel",
    "Italy",
#    "Jamaica",
    "Japan",
#    "Latvia",
#    "Lesotho",
    "Lithuania",
    "Luxembourg",
#    "Malaysia",
#    "Malta",
#    "Mexico",
#    "Mongolia",
#    "Namibia",
    "Netherlands",
    "New Zealand",
    "Norway",
#    "Panama",
#    "Papua New Guinea",
#    "Paraguay",
#    "Peru",
    "Philippines",
    "Poland",
    "Portugal",
    "Republic of Korea",
#    "Romania",
#    "Senegal",
#    "Serbia",
    "Singapore",
#    "Slovakia",
#    "Slovenia",
#    "South Africa",
    "Spain",
#    "Sri Lanka",
#    "Suriname",
    "Sweden",
    "Switzerland",
#    "Trinidad and Tobago",
#    "Tunisia",
    "Ukraine",
    "United Kingdom",
    "United States",
#    "Uruguay",
]

public_healthcare = [ # countries with universal healthcare determined by https://worldpopulationreview.com/countries/countries-with-universal-healthcare/
	"Albania",
	"Algeria",
	"Andorra",
	"Antigua and Barbuda",
	"Argentina",
	"Australia",
	"Austria",
	"Bahamas",
	"Bahrain",
	"Barbados",
	"Belarus",
	"Belgium",
	"Belize",
	"Bhutan",
	"Bolivia",
	"Bosnia and Herzegovina",
	"Botswana",
	"Brazil",
	"Brunei",
	"Bulgaria",
	"Burkina Faso",
	"Canada",
	"Chile",
	"China",
	"Colombia",
	"Cook Islands",
	"Costa Rica",
	"Croatia",
	"Cuba",
	"Cyprus",
	"Czechia",
	"Denmark",
	"Ecuador",
	"Eritrea",
	"Estonia",
	"Fiji",
	"Finland",
	"France",
	"Gabon",
	"Georgia",
	"Germany",
	"Ghana",
	"Greece",
	"Guernsey",
	"Guyana",
	# "Hong Kong",
	"Hungary",
	"Iceland",
	"Iran",
	"Israel",
	"Italy",
	"Jamaica",
	"Japan",
	"Jersey",
	"Kazakhstan",
	"Kiribati",
	"Kuwait",
	"Latvia",
	"Liechtenstein",
	"Lithuania",
	"Luxembourg",
	"Macau",
	"Macedonia",
	"Malaysia",
	"Maldives",
	"Malta",
	"Mauritius",
	"Mexico",
	"Moldova",
	"Monaco",
	"Montenegro",
	"Namibia",
	"Netherlands",
	"New Zealand",
	"Oman",
	"Pakistan",
	"Palau",
	"Panama",
	"Paraguay",
	"Peru",
	"Poland",
	"Portugal",
	"Qatar",
	"Republic of Korea",
	"Romania",
	"Russia",
	"Rwanda",
	"Saint Lucia",
	"Samoa",
	"San Marino",
	"Saudi Arabia",
	"Serbia",
	"Seychelles",
	"Singapore",
	"Slovakia", 
	"Slovenia",
	"Spain",
	"Sri Lanka",
	"Sweden",
	"Switzerland",
	"Thailand",
	"Timor-Leste",
	"Tonga",
	"Trinidad and Tobago",
	"Tunisia",
	"Turkey",
	"Tuvalu",
	"Ukraine",
	"United Arab Emirates",
	"United Kingdom",
	"Uruguay",
	"Uzbekistan",
	"Vanuatu",
	"Venezuela",
	"Zambia"
]

''' Data references '''

data_files = (
"SYB62_246_201907_Population Growth, Fertility and Mortality Indicators.csv",
"SYB63_325_202009_Expenditure on Health.csv",
"SYB63_154_202009_Health Personnel.csv"
)

data_fields = (
("Population annual rate of increase (percent)", "Total fertility rate (children per women)", "Infant mortality for both sexes (per 1,000 live births)", "Maternal mortality ratio (deaths per 100,000 population)", "Life expectancy at birth for both sexes (years)", "Life expectancy at birth for males (years)", "Life expectancy at birth for females (years)"),
("Current health expenditure", "Domestic general government health expenditure"),
("Health personnel: Physicians (number)", "Health personnel: Physicians (per 1000 population)", "Health personnel: Pharmacists (number)", "Health personnel: Pharmacists (per 1000 population)", "Health personnel: Nurses and midwives (number)", "Health personnel: Nurses and midwives personnel (per 1000 population)", "Health personnel: Dentists (number)", "Health personnel: Dentists (per 1000 population)")
)

relevant_data_fields = (
("Population annual rate of increase (percent)", "Total fertility rate (children per women)", "Infant mortality for both sexes (per 1,000 live births)", "Maternal mortality ratio (deaths per 100,000 population)", "Life expectancy at birth for both sexes (years)", "Life expectancy at birth for males (years)", "Life expectancy at birth for females (years)"),
("Current health expenditure", "Domestic general government health expenditure"),
("Health personnel: Physicians (number)","Health personnel: Pharmacists (number)",  "Health personnel: Nurses and midwives (number)", "Health personnel: Dentists (number)")
)

relevant_data_keys = (
("Population increase", "Fertility rate", "Infant mortality", "Maternal mortality", "Life expectancy at birth", "Life expectancy at birth for males", "Life expectancy at birth for females"),
("Current health expenditure", "Government health expenditure"),
("Physicians", "Pharmacists", "Nurses and midwives",  "Dentists")
)

population_percent_fields = (
"Population annual rate of increase (percent)",
"Total fertility rate (children per women)",
"Infant mortality for both sexes (per 1,000 live births)",
"Maternal mortality ratio (deaths per 100,000 population)"
)

percent_gdp_fields = (
"Current health expenditure", "Domestic general government health expenditure"
)

region_ids = (1,2,15,202,14,17,18,11,21,419,29,13,5,142,143,30,62,35,34,145,150,151,154,39,155,9,53,54,57,61)


attributes = [
"Infant mortality", 
"Maternal mortality", 
"Life expectancy at birth", 
"Life expectancy at birth for males", 
"Life expectancy at birth for females",
"Current health expenditure", 
"Government health expenditure",
"Physicians", 
"Pharmacists", 
"Nurses and midwives", 
"Dentists",
"population",
"gdp"
]



class country :

	def __init__ (self, country_name):

		#print(f"Creating {country_name}...")

		self.name = country_name
		self.population = 0
		self.gdp = 0

		for file in ("SYB63_1_202009_Population, Surface Area and Density.csv", "SYB63_230_202009_GDP and GDP Per Capita.csv"):
			with open(file, 'r+') as f:

				reader = csv.reader(f,delimiter=',')

				next(reader)
				next(reader)

				for row in reader:
					if row[1] == country_name and row[3] == "Population mid-year estimates (millions)":
						self.population = float(row[4])

					if row[1] == country_name and row[3] == "GDP in current prices (millions of US dollars)":
						self.gdp = float(row[4])
			f.close()

		for i, file in enumerate(data_files):
			with open (file, 'r+', encoding='iso-8859-1') as f:
				reader = csv.reader(f, delimiter=',')

				for j, field in enumerate(relevant_data_fields[i]):

					#print(f"Field: {field}")

					key = relevant_data_keys[i][j]
					value = 0
					year = 0

					f.seek(0)
					next(reader)
					next(reader)

					for row in reader:

						try:
							if row[1] == country_name and field in row[3] and int(row[2]) > year:

								#print(f"Row: {row}")

								year = int(row[2])
								value = float(row[4])

						except:
							print(f"Could not read: {row}")

					self.__dict__[key] = value

					if field in population_percent_fields:
						# print("pop percent")
						self.__dict__[key] *= self.population

					elif field in percent_gdp_fields:
						# print("gdp percent")
						self.__dict__[key] *= self.gdp

					if self.__dict__[key] < 0:
						self.__dict__[key] *= -1
				f.close()

	def __str__(self):
		print(self.name)
		for keys in relevant_data_keys:
			for key in keys:
				try:
					print(f"{key} = {self.__dict__[key]}")
				except:
					print(f"Could not print: {key}")

def one_dimension_array (data,length):
	array = np.zeros(shape=(length,))
	for n in range(length):
		array[n] = data.__dict__[attributes[n]]
	return array

def two_dimension_array (data, length, width):
	array = np.zeros(shape=(length,width))
	for m in range(width):
		for n in range(length):
			# print(f"{data[m].name} - >{data[m].__dict__[attributes[n]]}")
			array[n][m] = data[m].__dict__[attributes[n]]
	return array

def open_file(file_name):
	with open(file_name, 'rb') as fp:
		p = pickle.load(fp)
	return p

def save_file(file_name, file):
	with open(file_name, 'wb') as fp:
		pickle.dump(file,fp)

	for m in range(width):
		for n in range(length):
			X[n][m] = data[m].__dict__[attributes[n]]

def print_results(file_name):
	print("length of countries: ", len(countries))

	solution_list = [""]

	with open(file_name, "r+") as f:

		solution = f.read()

		i = 0
		for index in range(len(solution)):

			character = solution[index]

			if character == " ":
				i += 1
				solution_list.append("")

			else:
				solution_list[i] += character

		f.close()

	# for i in range (len(democracies_with_public_healthcare)):
	# 	print (f"{i} -> {democracies_with_public_healthcare[i]} -> {solution_list[i]}")

	mock_us = [0]*(len(countries)+2)
	us = country("United States of America")

	solution_total = 0
	for i in range(len(solution_list)-2) :
		solution_total += float(solution_list[i])

	for i in range(len(countries)):

		for j in range(len(solution_list)-2):

			if "Life expectancy" in attributes[i]:
				mock_us[i] += (float(countries[i][j]) * float(solution_list[j]))/solution_total

			else:
				mock_us[i] += (float(countries[i][j]) * float(solution_list[j]))


	print("\n RESULTS \n")

	for i, attribute in enumerate(attributes):

		print(f"{attribute} | US: {us.__dict__[attribute]} | MOCK US: {mock_us[i]}")

	print("\n DIFFERENCE \n")

	for i, attribute in enumerate(attributes):

		print(f"{attribute} | {( (us.__dict__[attribute] - mock_us[i])/us.__dict__[attribute] ) * 100}%")

def save_results(file_name):

	solution_list = [""]

	with open(file_name, "r+") as f:

		solution = f.read()

		i = 0
		for index in range(len(solution)):

			character = solution[index]

			if character == " ":
				i += 1
				solution_list.append("")

			else:
				solution_list[i] += character

		f.close()

	mock_us = [0]*(len(countries)+2)
	us = country("United States of America")

	solution_total = 0
	for i in range(len(solution_list)-2) :
		solution_total += float(solution_list[i])

	for i in range(len(countries)):

		for j in range(len(solution_list)-2):

			if "Life expectancy" in attributes[i]:
				mock_us[i] += (float(countries[i][j]) * float(solution_list[j]))/solution_total

			else:
				mock_us[i] += (float(countries[i][j]) * float(solution_list[j]))


	output_array = np.zeros(shape=(len(attributes),))

	for i, attribute in enumerate(attributes):
		output_array[i] = ( (us.__dict__[attribute] - mock_us[i])/us.__dict__[attribute] ) * 100

	return output_array

###############################################################################

democracies_with_public_healthcare = sorted(list(set(democracies).intersection(set(public_healthcare)))) # countries = democracies with universal healthcare

'''initialize'''

number_of_attributes = len(attributes)
number_of_countries = len(democracies_with_public_healthcare)

country_list = []
for d in democracies_with_public_healthcare:
	print(f"creating object for {d} ... ")
	c = country(d)
	country_list.append(country(d))
	#print(c)

print("Finished creating countries")

with open(country_file, 'wb') as f:
	pickle.dump(country_list,f)
	f.close()

number_of_solutions = 20

countries_data = open_file(country_file)

'''create numpy array'''

countries = two_dimension_array(countries_data, number_of_attributes, number_of_countries)
# print(countries)

''' create text file matrix '''

print(f"Number of countries: {number_of_countries}")

for i,democracy in enumerate(democracies_with_public_healthcare):
	print (f"{i} -> {democracy}")

solution = np.zeros(shape=(number_of_solutions,number_of_attributes))

for i in range(number_of_solutions):
	print(f"[{i+1}/{number_of_solutions}]")
	file = "solution_matrix_" + str(i) + ".txt"
	print_results(file)
	solution[i] = save_results(file)

transpose_solution = solution.transpose()

print("\n\nAVERAGE\n\n")

for i,attribute in enumerate(attributes):
	print(f"{attribute}: average = {transpose_solution[i].mean()}, std. dev. ={transpose_solution[i].std()}")