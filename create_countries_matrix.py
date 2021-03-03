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

country_file = "countrydata.txt"
unitedstates_file = "unitedstates.txt"

''' LISTS '''

democracies = [ # democracies determined by countries with at least a democracy score of 6 according to https://worldpopulationreview.com/countries/democracy-countries/
	"Argentina",
	"Australia",
	"Austria",
	"Belgium",
#	"Botswana",
	"Brazil",
	"Bulgaria",
	"Canada",
#	"Cape Verde",
#	"Chile",
#	"Colombia",
#	"Costa Rica",
	"Croatia",
	"Cyprus",
	"Czechia",
	"Denmark",
#	"Dominican Republic",
#	"Ecuador",
	"Estonia",
	"Finland",
	"France",
	"Germany",
#	"Ghana",
	"Greece",
#	"Hong Kong",
	"Hungary",
	"Iceland",
#	"India",
#	"Indonesia",
	"Ireland",
	"Israel",
	"Italy",
#	"Jamaica",
	"Japan",
#	"Latvia",
#	"Lesotho",
	"Lithuania",
	"Luxembourg",
#	"Malaysia",
#	"Malta",
#	"Mexico",
#	"Mongolia",
#	"Namibia",
	"Netherlands",
	"New Zealand",
	"Norway",
#	"Panama",
#	"Papua New Guinea",
#	"Paraguay",
#	"Peru",
	"Philippines",
	"Poland",
	"Portugal",
	"Republic of Korea",
#	"Romania",
#	"Senegal",
#	"Serbia",
	"Singapore",
#	"Slovakia",
#	"Slovenia",
#	"South Africa",
	"Spain",
#	"Sri Lanka",
#	"Suriname",
	"Sweden",
	"Switzerland",
#	"Trinidad and Tobago",
#	"Tunisia",
	"Ukraine",
	"United Kingdom",
	"United States",
#	"Uruguay",
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
"SYB63_1_202009_Population, Surface Area and Density.csv",
"SYB63_327_202009_International Migrants and Refugees.csv",
# "SYB62_246_201907_Population Growth, Fertility and Mortality Indicators.csv",
# "SYB61_253_Population Growth Rates in Urban areas and Capital cities.csv",
"SYB63_230_202009_GDP and GDP Per Capita.csv",
"SYB63_309_202009_Education.csv",
"SYB63_329_202009_Labour Force and Unemployment.csv",
"SYB63_328_202009_Intentional Homicides and Other Crimes.csv",
"SYB63_285_202009_Research and Development Staff.csv",
"SYB63_286_202009_GDP on R&D.csv",
"SYB63_315_202009_Water and Sanitation Services.csv",
"SYB63_314_202009_Internet Usage.csv"
)

data_fields = (
("Population mid-year estimates (millions)", "Population mid-year estimates for males (millions)", "Population mid-year estimates for females (millions)", "Sex ratio (males per 100 females)", "Population aged 0 to 14 years old (percentage)", "Population aged 60+ years old (percentage)","Population density"),
("International migrant stock: Both sexes (number)", "International migrant stock: Both sexes (% total population)", "International migrant stock: Male (% total Population)", "International migrant stock: Female (% total Population)"),
# ("Population annual rate of increase (percent)", "Total fertility rate (children per women)", "Infant mortality for both sexes (per 1,000 live births)", "Maternal mortality ratio (deaths per 100,000 population)", "Life expectancy at birth for both sexes (years)", "Life expectancy at birth for males (years)", "Life expectancy at birth for females (years)"),
# ("Urban population (percent)", "Urban population (percent growth rate per annum)", "Rural population (percent growth rate per annum)"),
("GDP in current prices (millions of US dollars)", "GDP per capita (US dollars)", "GDP in constant 2010 prices (millions of US dollars)", "GDP real rates of growth (percent)"),
("Students enrolled in primary education (thousands)", "Gross enrollment ratio - Primary (male)", "Gross enrollment ratio - Primary (female)"),
("Labour force participation - Total", "Unemployment rate - Total", "Labour force participation - Male", "Unemployment rate - Male", "Labour force participation - Female", "Unemployment rate - Female"),
("Intentional homicide rates per 100,000", "Percentage of male and female intentional homicide victims, Male", "Percentage of male and female intentional homicide victims, Female"),
("R & D personnel: Total (number in full-time equivalent)", "R & D personnel: Researchers - total (number in full-time equivalent)", "R & D personnel: Researchers - women (number in full-time equivalent)", "R & D personnel: Other supporting staff - total (number in full-time equivalent)", "R & D personnel: Total (number in full-time equivalent)"),
("Gross domestic expenditure on R & D: as a percentage of GDP (%)"),
("Safely managed drinking water sources, total (Proportion of population with access)", "Safely managed sanitation facilities, total (Proportion of population with access)"),
("Percentage of individuals using the internet",)
)

relevant_data_fields = (
("Population mid-year estimates (millions)", "Population aged 0 to 14 years old (percentage)", "Population aged 60+ years old (percentage)"),
("International migrant stock: Both sexes (number)",),
# ("Population annual rate of increase (percent)",),
# ("Urban population (percent)",),
("GDP in current prices (millions of US dollars)", "GDP per capita (US dollars)"),
("Students enrolled in primary education (thousands)",),
("Labour force participation - Total", "Unemployment rate - Total"),
("Intentional homicide rates per 100,000",),
("R & D personnel: Total (number in full-time equivalent)", "R & D personnel: Researchers - total (number in full-time equivalent)"),
("Gross domestic expenditure on R & D: as a percentage of GDP (%)",),
("Safely managed drinking water sources, total (Proportion of population with access)", "Safely managed sanitation facilities, total (Proportion of population with access)"),
("Percentage of individuals using the internet",)
)

population_percent_fields = (
"Population aged 0 to 14 years old (percentage)", "Population aged 60+ years old (percentage)",
# "Population annual rate of increase (percent)",
# "Urban population (percent)",
"GDP per capita (US dollars)",
"Labour force participation - Total", "Unemployment rate - Total",
"Safely managed drinking water sources, total (Proportion of population with access)", "Safely managed sanitation facilities, total (Proportion of population with access)",
"Percentage of individuals using the internet"
)

percent_gdp_fields = (
"Gross domestic expenditure on R & D: as a percentage of GDP (%)"
)

relevant_data_keys = (
("population", "population 0 to 14 years old", "population 60+"),
("immigrants",),
# ("population increase",),
# ("urban population",),
("GDP", "GDP per capita"),
("students",),
("labour force", "unemployment rate"),
("homicides",),
("R & D personnel", "researchers"),
("expenditure on R & D",),
("safe water", "sanitation"),
("internet",)
)

region_ids = (1,2,15,202,14,17,18,11,21,419,29,13,5,142,143,30,62,35,34,145,150,151,154,39,155,9,53,54,57,61)


attributes = [
"population",
# "population 0 to 14 years old",
"population 60+",
# "immigrants",
# "population increase",
# "urban population",
"gdp", 
# "GDP per capita",
# "students",
# "labour force",
# "unemployment rate",
# "homicides",
# "R & D personnel", 
"researchers",
# "expenditure on R & D",
"safe water",
#"sanitation",
#"internet"
]



class country :

	def __init__ (self, country_name):

		self.name = country_name
		self.population = 0
		self.gdp = 0

		for file in ("SYB63_1_202009_Population, Surface Area and Density.csv", "SYB63_230_202009_GDP and GDP Per Capita.csv"):
			with open(file, 'r+') as f:

				print("opened file...")

				reader = csv.reader(f,delimiter=',')

				next(reader)
				next(reader)

				for row in reader:
					if row[1] == country_name and row[3] == "Population mid-year estimates (millions)":
						self.population = float(row[4])

					if row[1] == country_name and row[3] == "GDP in current prices (millions of US dollars)":
						self.gdp = float(row[4])
			f.close()

		# print(self.name)
		# print(f"population = {self.population}")
		# print(f"gdp = {self.gdp}")

		i = 0
		for file in data_files:
			# print(file)
			try:
				f = pd.read_csv(file)
			except:
				# print(f"Could not read {file}")
				continue
			# if (file == "SYB63_314_202009_Internet Usage.csv" ):
			# 	print(file)
			j = 0
			for field in relevant_data_fields[i]:
				# print(f"i = {i}, j = {j}")
				key = relevant_data_keys[i][j]
				column = 4

				self.__dict__[key] = 0

				value = csv_search(f, field, country_name, column)
				if isinstance(value, int):
					self.__dict__[key] = value
				else:
					self.__dict__[key] = float(value.replace(',',''))

				if field in population_percent_fields:
					# print("pop percent")
					self.__dict__[key] *= self.population

				elif field in percent_gdp_fields:
					# print("gdp percent")
					self.__dict__[key] *= self.gdp

				if self.__dict__[key] < 0:
					self.__dict__[key] *= -1
				j+=1
			i+=1

def csv_search (df, condition, country_name, column):
	result = -1
	year = 0
	for i in range(2, len(df.index)):
		if (country_name in df.values[i, 1] and condition in df.values[i, 3]
		    and int(df.values[i, 2]) > year):
			year = int(df.values[i,2])
			result = df.values[i,column]
	return result

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

###############################################################################

democracies_with_public_healthcare = sorted(list(set(democracies).intersection(set(public_healthcare)))) # countries = democracies with universal healthcare

print(len(democracies_with_public_healthcare))

for i in range (len(democracies_with_public_healthcare)):
	print (f"{i} -> {democracies_with_public_healthcare[i]}")

'''initialize'''

N = len(attributes)
M = len(democracies_with_public_healthcare)

country_list = [country(d) for d in democracies_with_public_healthcare]

with open(country_file, 'wb') as f:
    pickle.dump(country_list,f)
    f.close()

with open(unitedstates_file, 'wb') as f:
    pickle.dump(country('United States of America'),f)
    f.close()


united_states_data = open_file(unitedstates_file)
countries_data = open_file(country_file)

'''create numpy arrays'''

united_states = one_dimension_array(united_states_data, N)

countries = two_dimension_array(countries_data, N, M)
# print(countries)

''' create text file matrix '''

file_name = "countries_matrix_" + str(N) + ".txt"

with open(file_name, "w+") as fp:
	i = 0
	for country in countries:
		for data in country:
			fp.write(f"{data} ")
		fp.write(f"-{united_states[i]}\n")
		i += 1
	fp.close()

print(f"Created a {N} x {M} matrix.")
